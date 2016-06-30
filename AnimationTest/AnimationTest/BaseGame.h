#pragma once
#include "CommonIncludes.h"
#include "InputManager.h"
#include "ThirdPersonCamera.h"
#include "Animation/ConstantBuffer.h"
#include "Animation/CBufferLayout.h"
#include "Animation/AnimatedModel.h"


class BaseGame
{
public:
	BaseGame(HINSTANCE hInstance, const wstring& WindowClassName, const wstring& windowTitle, int showCMD);
	virtual ~BaseGame() = default;

	//Core
	virtual void Initialize();
	virtual void Run();
	virtual void Update();
	virtual void Shutdown();

	//Accessors
	inline HWND GetWindowHandle() const { return mWindowHandle; }
	inline const wstring& GetWindowTitle() const { return mWindowTitle; }
	inline ID3D11Device1* D3DDevice() const { return mDirect3DDevice; }
	inline ID3D11DeviceContext1* D3DDevCon() const { return mDirect3DDeviceContext; }
	inline ID3D11RenderTargetView* BackBufferRTV() const { return mRenderTargetView; }
	inline ID3D11DepthStencilView* BackBufferDSV() const { return mDepthStencilView; }
	inline const D3D11_VIEWPORT& GetViewPort() const { return mViewPort; }
	inline IDXGISwapChain* SwapChain() const { return mSwapChain; }
	inline int GetScreenWidth() const { return mScreenWidth; }
	inline int GetScreenHeight() const { return mScreenHeight; }
	inline float GetAspectRatio() const { return static_cast<float>(static_cast<float>(mScreenWidth) / static_cast<float>(mScreenHeight)); }
	//For the particle system
	inline ID3D11Texture2D* DepthStencilTexture() const { return mDepthStencilBuffer; }

private:
	//Trilogy
	BaseGame(const BaseGame& rhs) = delete;
	BaseGame& operator=(const BaseGame& rhs) = delete;

	//-Win32-//
	//Win32 Message Callback Func
	static LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	//Win32 Helper Funcs
	void InitWin32();
	POINT CenterOfWindow(int windowWidth, int windowHeight);

	//Win32 DataMembers
	HWND mWindowHandle;
	WNDCLASSEX mWindowClass;
	HINSTANCE mhInstance;
	wstring mWindowClassName;
	wstring mWindowTitle;
	int mShowCMD;
	static const UINT DefaultScreenWidth;
	static const UINT DefaultScreenHeight;
	UINT mScreenWidth;
	UINT mScreenHeight;
	GameClock mClock;

	//-D3D-//
	//D3D Helper Funcs
	void InitD3D11();

	//D3D DataMembers
	ID3D11Device1* mDirect3DDevice = nullptr;
	ID3D11DeviceContext1* mDirect3DDeviceContext = nullptr;
	IDXGISwapChain1* mSwapChain = nullptr;


	D3D11_TEXTURE2D_DESC mBackBufferDesc;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11DepthStencilView* mDepthStencilView = nullptr;
	ID3D11Texture2D* mDepthStencilBuffer = nullptr;
	D3D11_VIEWPORT mViewPort;
	D3D_FEATURE_LEVEL mFeatureLevel;


	//D3D Misc
	static const DirectX::XMVECTORF32 DefaultClearColor;
	static const UINT DefaultFrameRate;
	static const UINT DefaultMSAASampleCount;
	UINT mFrameRate;
	UINT mMSAASampleCount;
	UINT mMSAAQualityLevels;
	bool mIsFullScreen;
	bool mIsDepthStencilBufferEnabled;
	bool mMSAAEnabled;

	//-Singletons-//
	GameTime* GameTime = nullptr;
	ThirdPersonCamera* ThirdPersonCamera = nullptr;
	InputManager* InputManager = nullptr;

	//-Per Frame Constant Buffer-//
	ConstantBuffer<cbPerFrameLightCameraData> mScenePerFrameCameraConstBuffer;


	static float mElapsedTime;

	//Singleton Helper Funcs
	void InitSingletons();
	void UpdateSingletons();
	void ShutdownSingletons();


	AnimatedModel* mAnimatedModel;


	void InitModel();
	void UpdateRenderer();
	void UpdatePerFrameConstantBuffer();
};