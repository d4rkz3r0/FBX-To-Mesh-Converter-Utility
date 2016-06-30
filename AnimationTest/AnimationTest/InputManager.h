#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#pragma once
#include <dinput.h>
#include <Gamepad.h>
#include <memory>
#include "Mouse.h"
#include "Keyboard.h"
#include "Singleton.h"
#include "CommonIncludes.h"

using std::unique_ptr;
using std::make_unique;
using DirectX::GamePad;


class InputManager : public Singleton<InputManager>
{
public:
	friend class Singleton<InputManager>;

	//Core Overrides
	void Initialize(HWND windowHandle, HINSTANCE windowInstance);
	void Update();
	void Shutdown();
	static bool isInitialized;

#pragma region KBM Public Interface
	Keyboard* GetKeyboard();
	Mouse* GetMouse();
#pragma endregion

#pragma region GamePad Public Interface
	GamePad::State GetGamePad();
	bool isAButtonDown();
	bool isBButtonDown();
	bool isYButtonDown();
	bool isXButtonDown();
	bool isLeftThumbStickBeingPressedUpwardsThisFrame();
	bool isLeftThumbStickBeingPressedDownwardsThisFrame();
	float GamePadLeftStickXValue();
	float GamePadLeftStickYValue();
	float GamePadRightStickXValue();
	float GamePadRightStickYValue();
	void RumbleGamePad(float rumbleStrength, float rumbleDuration);
	void SetPlayerButtonConfig(string ability1Name, string ability2Name, string ability3Name, string attackChainName);

	//GamePad Win32 Functions
	void EnableGamePad();
	void DisableGamePad();

#pragma endregion

	//InputComponent <-> InputManager Communication Functions//
	void Register(UINT& registrationID, string attack1Name, string attack2Name, string attack3Name, string attack4Name);
	void DeRegister(UINT& registrationID);

private:
	InputManager() = default;
	virtual ~InputManager() = default;
	InputManager(InputManager const& rhs) = delete;
	InputManager& operator=(InputManager const& rhs) = delete;


	//Supported Devices
	LPDIRECTINPUT8 mDirectInput;
	unique_ptr<Mouse> mMouse;
	unique_ptr<Keyboard> mKeyboard;
	unique_ptr<GamePad> mGamepad;
	unique_ptr<GamePad::ButtonStateTracker> mGamepadTracker;
	unique_ptr<GamePad::ThumbSticks> mThumbStickTracker;

	//Main Loop Funcs
	void UpdateDevices();
	void UpdateGamePadTracker();
	void ProcessInputComponents();
	void ProcessPlayerInput();
	void ProcessApplicationInput();

	//GamePad Config
	//unique_ptr<GameCommand> AButton;
	//unique_ptr<GameCommand> YButton;
	//unique_ptr<GameCommand> BButton;
	//unique_ptr<GameCommand> XButton;
	//unique_ptr<GameCommand> LeftThumbStickUp;
	//unique_ptr<GameCommand> LeftThumbStickDown;
	//unique_ptr<GameCommand> LeftThumbStickLeft;
	//unique_ptr<GameCommand> LeftThumbStickRight;

	//Command Bank
	//unique_ptr<CommandBank> mCommandBank;
	//Input Component Bank
	//map<UINT, unique_ptr<InputComponent>> mRegisteredInputComponents;
};