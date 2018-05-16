#pragma once

#include "qeheader.h"

class QeTimer
{
public:
	int timerMilliSecond;
	std::chrono::steady_clock::time_point lastTime;

	void setTimer( int timerMilliSecond);
	bool checkTimer( int & passMilliSecond);
};