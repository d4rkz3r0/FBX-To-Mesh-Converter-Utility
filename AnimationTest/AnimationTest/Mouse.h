#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#pragma once
#include <dinput.h>

enum MouseButtons
{
	MouseButtonLeft = 0,
	MouseButtonRight = 1,
	MouseButtonMiddle = 2,
	MouseButtonX1 = 3
};

class Mouse
{

public:
	Mouse();
	~Mouse();

	LPDIMOUSESTATE GetCurrentState();
	LPDIMOUSESTATE GetLastState();

	void Initialize(HWND windowHandle, LPDIRECTINPUT8 directInput);
	void Update();

	long X() const;
	long Y() const;
	long Wheel() const;

	bool IsButtonUp(MouseButtons button) const;
	bool IsButtonDown(MouseButtons button) const;
	bool WasButtonUp(MouseButtons button) const;
	bool WasButtonDown(MouseButtons button) const;
	bool WasButtonPressedThisFrame(MouseButtons button) const;
	bool WasButtonReleasedThisFrame(MouseButtons button) const;
	bool IsButtonHeldDown(MouseButtons button) const;

private:
	Mouse(const Mouse& rhs) = delete;

	//DirectInput
	LPDIRECTINPUTDEVICE8 mDirectInputDevice;
	DIMOUSESTATE mCurrentState;
	DIMOUSESTATE mLastState;

	long mX;
	long mY;
	long mWheel;
};
