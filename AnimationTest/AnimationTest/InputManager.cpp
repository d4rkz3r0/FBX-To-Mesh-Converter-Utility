#include "InputManager.h"
#include "CommonIncludes.h"
#include "ThirdPersonCamera.h"

bool InputManager::isInitialized = false;

namespace
{
	const int PLAYER_ONE = 0;
}

void InputManager::Initialize(HWND windowHandle, HINSTANCE windowInstance)
{
	if (FAILED(DirectInput8Create(windowInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID*>(&mDirectInput), nullptr)))
	{
		throw Exception("InputManager::Initialze()::DirectInput8Create() failed.");
	}

	mKeyboard = make_unique<Keyboard>();
	mKeyboard->Initialize(windowHandle, mDirectInput);

	mMouse = make_unique<Mouse>();
	mMouse->Initialize(windowHandle, mDirectInput);

	mGamepad = make_unique<GamePad>();
	mGamepadTracker = make_unique<GamePad::ButtonStateTracker>();
	mThumbStickTracker = make_unique<GamePad::ThumbSticks>();


	isInitialized = true;
}

void InputManager::Update()
{
	UpdateDevices();
	//ProcessPlayerInput(); Commented out for PerForce since player movement is being implemented elsewhere.
	ProcessInputComponents();
	ProcessApplicationInput();
}

void InputManager::UpdateDevices()
{
	mKeyboard->Update();
	mMouse->Update();
	UpdateGamePadTracker();
}

void InputManager::ProcessPlayerInput()
{

}

void InputManager::ProcessInputComponents()
{

}

void InputManager::ProcessApplicationInput()
{
	if (GetKeyboard()->IsKeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(NULL);
	}
}


void InputManager::Shutdown()
{

	ReleaseObject(mDirectInput);
	DestroyInstance();
}

void InputManager::Register(UINT& registrationID, string attack1Name, string attack2Name, string attack3Name, string attack4Name)
{

}

void InputManager::DeRegister(UINT& registrationID)
{
	
}

Keyboard* InputManager::GetKeyboard()
{
	return mKeyboard.get();
}

Mouse* InputManager::GetMouse()
{
	return mMouse.get();
}

GamePad::State InputManager::GetGamePad()
{
	return mGamepad->GetState(PLAYER_ONE);
}

float InputManager::GamePadLeftStickXValue()
{
	return mGamepad->GetState(PLAYER_ONE).thumbSticks.leftX;
}

float InputManager::GamePadLeftStickYValue()
{
	return mGamepad->GetState(PLAYER_ONE).thumbSticks.leftY;
}

float InputManager::GamePadRightStickXValue()
{
	return mGamepad->GetState(PLAYER_ONE).thumbSticks.rightX;
}

float InputManager::GamePadRightStickYValue()
{
	return mGamepad->GetState(PLAYER_ONE).thumbSticks.rightY;
}

bool InputManager::isAButtonDown()
{
	return mGamepadTracker->a == GamePad::ButtonStateTracker::PRESSED ? true : false;
}

bool InputManager::isBButtonDown()
{
	return mGamepadTracker->b == GamePad::ButtonStateTracker::PRESSED ? true : false;
}

bool InputManager::isYButtonDown()
{
	return mGamepadTracker->y == GamePad::ButtonStateTracker::PRESSED ? true : false;
}

bool InputManager::isXButtonDown()
{
	return mGamepadTracker->x == GamePad::ButtonStateTracker::PRESSED ? true : false;
}

bool InputManager::isLeftThumbStickBeingPressedUpwardsThisFrame()
{
	return GetGamePad().IsLeftThumbStickUp();
}

bool InputManager::isLeftThumbStickBeingPressedDownwardsThisFrame()
{
	return GetGamePad().IsLeftThumbStickDown();
}

void InputManager::UpdateGamePadTracker()
{
	auto gamePadState = mGamepad->GetState(PLAYER_ONE);
	if (gamePadState.IsConnected())
	{
		mGamepadTracker->Update(gamePadState);
	}
}

void InputManager::EnableGamePad()
{
	if (mGamepad->GetState(PLAYER_ONE).IsConnected())
	{
		mGamepadTracker->Reset();
		mGamepad->Resume();
	}
}

void InputManager::DisableGamePad()
{
	mGamepad->Suspend();
}


void InputManager::RumbleGamePad(float rumbleStrength, float rumbleDuration)
{
	float rumbleTimer = rumbleDuration;
	while (rumbleTimer > 0.0f)
	{
		mGamepad->SetVibration(PLAYER_ONE, rumbleStrength, rumbleStrength);
		rumbleTimer -= static_cast<float>(GameTime::GetInstance()->DeltaTime());
		if (rumbleTimer <= 0.0f)
		{
			mGamepad->SetVibration(PLAYER_ONE, 0.0f, 0.0f);
			break;
		}
	}
}