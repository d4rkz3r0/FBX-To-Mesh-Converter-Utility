#pragma once
#include <windows.h>
#include "GameTime.h"

//Forward Declaration
class GameTime;

class GameClock
{
public:
	GameClock();
	const LARGE_INTEGER& StartTime() const;
	const LARGE_INTEGER& CurrentTime() const;
	const LARGE_INTEGER& LastTime() const;

	void ResetClock();
	double GetFrequency() const;
	void GetTime(LARGE_INTEGER& time) const;
	void UpdateTime();

private:
	GameClock(const GameClock& rhs);
	GameClock& operator=(const GameClock& rhs) = delete;

	LARGE_INTEGER mGameStartTime;
	LARGE_INTEGER mCurrentTime;
	LARGE_INTEGER mLastTime;
	double mFrequency;
};