#pragma once

#include "qeheader.h"

class QeTimer
{
public:
	int timerMilliSecond=0;
	std::chrono::steady_clock::time_point lastTime;

	std::chrono::steady_clock::time_point getNowTime();
	void initTime();
	void setTimer( int timerMilliSecond);
	int getPassTime();
	bool checkTimer( int & passMilliSecond);
};