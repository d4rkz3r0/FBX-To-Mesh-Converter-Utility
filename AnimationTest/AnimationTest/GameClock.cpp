#include "GameClock.h"
#include "Exception.h"


GameClock::GameClock() : mGameStartTime(), mCurrentTime(), mLastTime(), mFrequency()
{
	mFrequency = GetFrequency();
	ResetClock();
}

double GameClock::GetFrequency() const
{
	LARGE_INTEGER frequency;

	if (QueryPerformanceFrequency(&frequency) == false)
	{
		throw Exception("QueryPerformanceFrequency() failed.");
	}

	return static_cast<double>(frequency.QuadPart);
}

const LARGE_INTEGER& GameClock::StartTime() const
{
	return mGameStartTime;
}

const LARGE_INTEGER& GameClock::CurrentTime() const
{
	return mCurrentTime;
}

const LARGE_INTEGER& GameClock::LastTime() const
{
	return mLastTime;
}

void GameClock::ResetClock()
{
	GetTime(mGameStartTime);
	mCurrentTime = mGameStartTime;
	mLastTime = mCurrentTime;
}


void GameClock::GetTime(LARGE_INTEGER& time) const
{
	QueryPerformanceCounter(&time);
}

void GameClock::UpdateTime()
{
	GetTime(mCurrentTime);
	GameTime::GetInstance()->SetTotalGameTime((mCurrentTime.QuadPart - mGameStartTime.QuadPart) / mFrequency);
	GameTime::GetInstance()->SetDeltaTime((mCurrentTime.QuadPart - mLastTime.QuadPart) / mFrequency);

	mLastTime = mCurrentTime;
}