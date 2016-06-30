#include "GameTime.h"
#include <windows.h>
#include <sstream>

void GameTime::SetTotalGameTime(double totalTime)
{
	mTotalGameTime = totalTime;
}

void GameTime::SetDeltaTime(double deltaTime)
{
	mDeltaTime = deltaTime;
}

double GameTime::DeltaTime()
{
	return mDeltaTime;
}

double GameTime::TotalGameTime()
{
	return mTotalGameTime;
}

void GameTime::UpdateFPS(void* windowHandle, wstring windowTitleText)
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((mTotalGameTime - timeElapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frameCount);
		std::wostringstream OSS;
		OSS << windowTitleText << L"   " << L"FPS: " << fps;
		SetWindowText(static_cast<HWND>(windowHandle), OSS.str().c_str());

		//Reset
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

void GameTime::Shutdown()
{
	DestroyInstance();
}