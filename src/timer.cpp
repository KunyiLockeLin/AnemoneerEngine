#include "header.h"

std::chrono::steady_clock::time_point QeTimer::getNowTime() { return std::chrono::high_resolution_clock::now(); }

void QeTimer::initTime() { lastTime = getNowTime(); }

void QeTimer::setTimer(int _timerMilliSecond) {
    timerMilliSecond = _timerMilliSecond;
    initTime();
}

int QeTimer::getPassTime() {
    std::chrono::steady_clock::time_point currentTime = getNowTime();
    return int(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count());
}

bool QeTimer::checkTimer(int &passMilliSecond) {
    passMilliSecond = getPassTime();

    if (passMilliSecond >= timerMilliSecond) {
        lastTime = getNowTime();
        return true;
    }
    return false;
}
