#include "BaseGame.h"
#include "GameClock.h"
#include "GameTime.h"
#include "Animation/CBufferLayout.h"
#include "Converters/OBJConverter.h"
#include "Converters/FBXConverter.h"

float BaseGame::mElapsedTime = 0.0f;
const XMVECTORF32 BaseGame::DefaultClearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
const UINT BaseGame::DefaultScreenWidth = 1600;
const UINT BaseGame::DefaultScreenHeight = 900;
const UINT BaseGame::DefaultFrameRate = 60;
const UINT BaseGame::DefaultMSAASampleCount = 1;

BaseGame::BaseGame(HINSTANCE hInstance, const wstring& WindowClassName, const wstring& windowTitle, int showCMD) :
	mWindowHandle(), mWindowClass(), mhInstance(hInstance), mWindowClassName(WindowClassName),
	mWindowTitle(windowTitle), mShowCMD(showCMD), mScreenWidth(DefaultScreenWidth), mScreenHeight(DefaultScreenHeight),
	mDirect3DDevice(nullptr), mDirect3DDeviceContext(nullptr), mDepthStencilView(nullptr), mDepthStencilBuffer(nullptr),
	mViewPort(), mFeatureLevel(D3D_FEATURE_LEVEL_9_1), mFrameRate(DefaultFrameRate), mMSAASampleCount(DefaultMSAASampleCount),
	mMSAAQualityLevels(0), mIsFullScreen(false), mIsDepthStencilBufferEnabled(true), mMSAAEnabled(true)
{
	mClock.ResetClock();
}

void BaseGame::InitWin32()
{
	ZeroMemory(&mWindowClass, sizeof(mWindowClass));
	mWindowClass.cbSize = sizeof(WNDCLASSEX);
	mWindowClass.style = CS_CLASSDC;
	mWindowClass.lpfnWndProc = WndProc;
	mWindowClass.hInstance = mhInstance;
	mWindowClass.lpszClassName = mWindowClassName.c_str();
	mWindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mWindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mWindowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);

	RECT windowRectangle = { 0, 0, static_cast<LONG>(mScreenWidth), static_cast<LONG>(mScreenHeight) };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	RegisterClassEx(&mWindowClass);

	POINT centerOfWindow = CenterOfWindow(mScreenWidth, mScreenHeight);
	mWindowHandle = CreateWindow(mWindowClassName.c_str(), mWindowTitle.c_str(), WS_OVERLAPPEDWINDOW,
		centerOfWindow.x, centerOfWindow.y, windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, mhInstance, nullptr);

	ShowWindow(mWindowHandle, mShowCMD);
	UpdateWindow(mWindowHandle);
}

void BaseGame::InitD3D11()
{
	HRESULT hr;
	UINT deviceCreationFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dDevCon = nullptr;

	if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceCreationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &d3dDevice, &mFeatureLevel, &d3dDevCon)))
	{
		throw Exception("BaseGame::InitD3D11()->D3D11CreateDevice() failed.", hr);
	}

	if (FAILED(hr = d3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDirect3DDevice))))
	{
		throw Exception("BaseGame::InitD3D11()BaseGame::D3DDevice->QueryInterface() failed.", hr);
	}

	if (FAILED(hr = d3dDevCon->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mDirect3DDeviceContext))))
	{
		throw Exception("BaseGame::InitD3D11()BaseGame::D3DDeviceCon->QueryInterface() failed.", hr);
	}

	ReleaseObject(d3dDevice);
	ReleaseObject(d3dDevCon);

	mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMSAASampleCount, &mMSAAQualityLevels);

	if (mMSAAQualityLevels == 0)
	{
		throw Exception("BaseGame::InitD3D11(), MSAA not supported.", hr);
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMem(swapChainDesc);
	swapChainDesc.Width = mScreenWidth;
	swapChainDesc.Height = mScreenHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (mMSAAEnabled)
	{
		swapChainDesc.SampleDesc.Count = mMSAASampleCount;
		swapChainDesc.SampleDesc.Quality = mMSAAQualityLevels - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIDevice* dxgiDevice = nullptr;
	if (FAILED(hr = mDirect3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))))
	{
		throw Exception("BaseGame::InitD3D11()BaseGame::D3DDevice->QueryInterface() failed.", hr);
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter))))
	{
		ReleaseObject(dxgiDevice);
		throw Exception("BaseGame::InitD3D11()BaseGame::D3DDevice->GetParent() failed.", hr);
	}

	IDXGIFactory2* dxgiFactory = nullptr;
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory))))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		throw Exception("BaseGame::InitD3D11()BaseGame::D3DDevice->GetParent() failed.", hr);
	}

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc;
	ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
	fullScreenDesc.RefreshRate.Numerator = mFrameRate;
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.Windowed = !mIsFullScreen;

	//DXGI v1.2 Function
	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle, &swapChainDesc, &fullScreenDesc, nullptr, &mSwapChain)))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);
		throw Exception("IDXGIDeviceBaseGame::CreateSwapChainForHwnd() failed.", hr);
	}

	ReleaseObject(dxgiDevice);
	ReleaseObject(dxgiAdapter);
	ReleaseObject(dxgiFactory);

	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		throw Exception("IDXGISwapChainBaseGame::GetBuffer() failed.", hr);
	}



	backBuffer->GetDesc(&mBackBufferDesc);

	if (FAILED(hr = mDirect3DDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
	{
		ReleaseObject(backBuffer);
		throw Exception("IDXGIDeviceBaseGame::CreateRenderTargetView() failed.", hr);
	}



	ReleaseObject(backBuffer);

	if (mIsDepthStencilBufferEnabled)
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.Width = mScreenWidth;
		depthStencilDesc.Height = mScreenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(DSVDesc));
		DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;

		if (mMSAAEnabled)
		{
			depthStencilDesc.SampleDesc.Count = mMSAASampleCount;
			depthStencilDesc.SampleDesc.Quality = mMSAAQualityLevels - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		if (FAILED(hr = mDirect3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer)))
		{
			throw Exception("IDXGIDeviceBaseGame::CreateTexture2D() failed.", hr);
		}

		if (FAILED(hr = mDirect3DDevice->CreateDepthStencilView(mDepthStencilBuffer, &DSVDesc, &mDepthStencilView)))
		{
			throw Exception("IDXGIDeviceBaseGame::CreateDepthStencilView() failed.", hr);
		}

	}

	//ViewPort
	mViewPort.TopLeftX = 0.0f;
	mViewPort.TopLeftY = 0.0f;
	mViewPort.Width = static_cast<float>(mScreenWidth);
	mViewPort.Height = static_cast<float>(mScreenHeight);
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;

	mDirect3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mDirect3DDeviceContext->RSSetViewports(1, &mViewPort);
}

void BaseGame::Initialize()
{
	InitWin32();
	InitD3D11();
	InitSingletons();
	mScenePerFrameCameraConstBuffer.Initialize(mDirect3DDevice);

	FBXConverter fbxconverter;
	OBJConverter objconverter;

	InitModel();

}

void BaseGame::Run()
{
	Initialize();

	//Win32 Msg Loop
	MSG msg; ZeroMem(msg);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}
	Shutdown();
}

LRESULT WINAPI BaseGame::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(NULL);
		return EXIT_SUCCESS;
	}
	case WM_SYSKEYDOWN:
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			if ((lParam & (1 << 29)) != 0)
			{
				//RenderController::GetInstance()->ResizeBuffersFullScreen();
			}
		}
		break;
		}
		break;
	}

	case WM_ACTIVATE:
	{
		
		break;
	}
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}


void BaseGame::Update()
{
	UpdateSingletons();
	mElapsedTime += GameTime::GetInstance()->DeltaTime();
	UpdateRenderer();






}

void BaseGame::Shutdown()
{
	mScenePerFrameCameraConstBuffer.ShutDown();
	delete mAnimatedModel;

	ShutdownSingletons();
	ReleaseObject(mRenderTargetView);
	ReleaseObject(mDepthStencilView);
	ReleaseObject(mDepthStencilBuffer);
	ReleaseObject(mSwapChain);

	if (mDirect3DDeviceContext != nullptr)
	{
		mDirect3DDeviceContext->ClearState();
	}


	ReleaseObject(mDirect3DDeviceContext);
	ReleaseObject(mDirect3DDevice);
	UnregisterClass(mWindowClassName.c_str(), mWindowClass.hInstance);

	PostQuitMessage(NULL);
}

void BaseGame::InitSingletons()
{
	GameTime = GameTime::GetInstance();
	ThirdPersonCamera = ThirdPersonCamera::GetInstance();
	InputManager = InputManager::GetInstance();
	InputManager::GetInstance()->Initialize(mWindowHandle, mhInstance);
	ThirdPersonCamera::GetInstance()->Initialize(GetAspectRatio());
}

void BaseGame::UpdateSingletons()
{
	mClock.UpdateTime();
	GameTime::GetInstance()->UpdateFPS(mWindowHandle, mWindowTitle);
	ThirdPersonCamera::GetInstance()->Pie();
	InputManager::GetInstance()->Update();
	ThirdPersonCamera::GetInstance()->Update();
}

void BaseGame::ShutdownSingletons()
{
	GameTime::GetInstance()->Shutdown();
	ThirdPersonCamera::GetInstance()->Shutdown();
	InputManager::GetInstance()->Shutdown();
}

void BaseGame::InitModel()
{
	mAnimatedModel = new AnimatedModel();
	mAnimatedModel->Initialize(mDirect3DDevice, mDirect3DDeviceContext);
	mAnimatedModel->LoadSkinnedModel(string("character.fbx"));
}

void BaseGame::UpdateRenderer()
{
	mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&DefaultClearColor));
	mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);



	UpdatePerFrameConstantBuffer();
	if (InputManager::GetInstance()->GetKeyboard()->IsKeyHeldDown(DIK_UPARROW) ||
		InputManager::GetInstance()->GetKeyboard()->IsKeyHeldDown(DIK_DOWNARROW) ||
		InputManager::GetInstance()->GetKeyboard()->IsKeyHeldDown(DIK_LEFTARROW) ||
		InputManager::GetInstance()->GetKeyboard()->IsKeyHeldDown(DIK_RIGHTARROW))
	{
		mAnimatedModel->mAnimator->SetAnimIndex(2);
	}
	else
	{
		mAnimatedModel->mAnimator->SetAnimIndex(1);
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_O))
	{
		mAnimatedModel->mAnimator->SetAnimIndex(0);
	}

	mAnimatedModel->DrawMe(mElapsedTime, XMMatrixIdentity());


	HRESULT hr = mSwapChain->Present(0, 0);
	if (FAILED(hr))
	{
		throw Exception("IDXGISwapChain::Present() failed.", hr);
	}
}

void BaseGame::UpdatePerFrameConstantBuffer()
{
	cbPerFrameLightCameraData scenePerFrameCamInfo;
	scenePerFrameCamInfo.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	scenePerFrameCamInfo.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	scenePerFrameCamInfo.LightPosition = XMFLOAT3(0.0f, 20.0f, -60.0f);
	scenePerFrameCamInfo.LightRadius = 50000.0f;
	XMStoreFloat3(&scenePerFrameCamInfo.CameraPosition, ThirdPersonCamera::GetInstance()->GetPositionXM());
	mScenePerFrameCameraConstBuffer.Data = scenePerFrameCamInfo;
	mScenePerFrameCameraConstBuffer.ApplyChanges(D3DDevCon());
	auto cBufferPerFrame = mScenePerFrameCameraConstBuffer.Buffer();
	mDirect3DDeviceContext->VSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
	mDirect3DDeviceContext->PSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
}

POINT BaseGame::CenterOfWindow(int windowWidth, int windowHeight)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT centerOfWindow;
	centerOfWindow.x = (screenWidth - windowWidth) / 2;
	centerOfWindow.y = (screenHeight - windowHeight) / 2;

	return centerOfWindow;
}