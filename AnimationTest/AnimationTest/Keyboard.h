#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#pragma once
#include <dinput.h>

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	const byte* CurrentState() const;
	const byte* LastState() const;

	void Initialize(HWND windowHandle, LPDIRECTINPUT8 directInput);
	void Update();
	//void Shutdown();

	bool IsKeyUp(byte key) const;
	bool IsKeyDown(byte key) const;
	bool WasKeyUp(byte key) const;
	bool WasKeyDown(byte key) const;
	bool WasKeyPressedThisFrame(byte key) const;
	bool WasKeyReleasedThisFrame(byte key) const;
	bool IsKeyHeldDown(byte key) const;

private:
	Keyboard(const Keyboard& rhs) = delete;
	
	//DirectInput
	IDirectInputDevice8* mDirectInputDevice = nullptr;
	static const int KeyCount = 256;
	byte mCurrentState[KeyCount];
	byte mLastState[KeyCount];
};