//Simple Interface to access what the GameClock computes based on the
//Windows High Resolution Timer.
#pragma once
#include "Singleton.h"
#include <string>
using std::wstring;

class GameTime : public Singleton<GameTime>
{
public:
	friend class Singleton<GameTime>;

	//Core
	double TotalGameTime();
	double DeltaTime();

	//Mutators
	void SetTotalGameTime(double totalTime);
	void SetDeltaTime(double deltaTime);

	//Misc
	void UpdateFPS(void* windowHandle, wstring windowTitleText);
	void Shutdown();


private:
	GameTime() = default;
	virtual ~GameTime() = default;
	GameTime(GameTime const& rhs) = delete;
	GameTime& operator=(GameTime const& rhs) = delete;

	//DataMembers
	double mTotalGameTime;
	double mDeltaTime;
};