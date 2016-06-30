#include "Keyboard.h"
#include "CommonIncludes.h"

Keyboard::Keyboard() : mDirectInputDevice(nullptr)
{
	ZeroMem(mCurrentState);
	ZeroMem(mLastState);
}

Keyboard::~Keyboard()
{
	if (mDirectInputDevice != nullptr)
	{
		mDirectInputDevice->Unacquire();
		ReleaseObject(mDirectInputDevice);
	}
}

const byte* Keyboard::CurrentState() const
{
	return mCurrentState;
}

const byte* Keyboard::LastState() const
{
	return mLastState;
}

void Keyboard::Initialize(HWND windowHandle, LPDIRECTINPUT8 directInput)
{
	if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &mDirectInputDevice, nullptr)))
	{
		throw Exception("Keyboard::Initialize() failed.");
	}
	mDirectInputDevice->SetDataFormat(&c_dfDIKeyboard);
	mDirectInputDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	mDirectInputDevice->Acquire();
}

void Keyboard::Update()
{
	if (mDirectInputDevice != nullptr)
	{
		memcpy(mLastState, mCurrentState, sizeof(mCurrentState));
		if (FAILED(mDirectInputDevice->GetDeviceState(sizeof(mCurrentState), static_cast<LPVOID>(mCurrentState))))
		{
			if (SUCCEEDED(mDirectInputDevice->Acquire()))
			{
				mDirectInputDevice->GetDeviceState(sizeof(mCurrentState), static_cast<LPVOID>(mCurrentState));
			}
		}
	}
}

bool Keyboard::IsKeyUp(byte key) const
{
	return ((mCurrentState[key] & 0x80) == 0);
}

bool Keyboard::IsKeyDown(byte key) const
{
	return ((mCurrentState[key] & 0x80) != 0);
}

bool Keyboard::WasKeyUp(byte key) const
{
	return ((mLastState[key] & 0x80) == 0);
}

bool Keyboard::WasKeyDown(byte key) const
{
	return ((mLastState[key] & 0x80) != 0);
}

bool Keyboard::WasKeyPressedThisFrame(byte key) const
{
	return (IsKeyDown(key) && WasKeyUp(key));
}

bool Keyboard::WasKeyReleasedThisFrame(byte key) const
{
	return (IsKeyUp(key) && WasKeyDown(key));
}

bool Keyboard::IsKeyHeldDown(byte key) const
{
	return (IsKeyDown(key) && WasKeyDown(key));
}