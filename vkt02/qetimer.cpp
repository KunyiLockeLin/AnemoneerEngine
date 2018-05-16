#include "qeheader.h"


void QeTimer::setTimer(int _timerMilliSecond) {
	timerMilliSecond = _timerMilliSecond;
	lastTime = std::chrono::high_resolution_clock::now();
}

bool QeTimer::checkTimer(int & passMilliSecond) {
	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	passMilliSecond = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	if (passMilliSecond >= timerMilliSecond) {
		lastTime = currentTime;
		return true;
	}
	return false;
}