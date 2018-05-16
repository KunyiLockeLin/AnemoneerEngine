#include "qeheader.h"


std::chrono::steady_clock::time_point QeTimer::getNowTime() {
	return std::chrono::high_resolution_clock::now();
}

void QeTimer::setTimer(int _timerMilliSecond) {
	timerMilliSecond = _timerMilliSecond;
	lastTime = getNowTime();
}

bool QeTimer::checkTimer(int & passMilliSecond) {
	std::chrono::steady_clock::time_point currentTime = getNowTime();
	passMilliSecond = int(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count());

	if (passMilliSecond >= timerMilliSecond) {
		lastTime = currentTime;
		return true;
	}
	return false;
}