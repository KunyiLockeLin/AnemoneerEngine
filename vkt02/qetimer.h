#pragma once

#include "qeheader.h"

class QeTimer
{
public:
	int timerMilliSecond;
	std::chrono::steady_clock::time_point lastTime;

	std::chrono::steady_clock::time_point getNowTime();
	void setTimer( int timerMilliSecond);
	bool checkTimer( int & passMilliSecond);
};