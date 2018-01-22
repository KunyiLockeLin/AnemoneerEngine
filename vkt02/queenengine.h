#pragma once

#include "qeheader.h"

class QueenEngine
{
public:
	QueenEngine(QeGlobalKey& _key) {}
	~QueenEngine() {}
	void run();

	std::string activityName = "";
	QeActivity* currentActivity = nullptr;
	bool bClosed = false;
	bool bRestart = false;
	int currentFPS = 0;
	std::chrono::steady_clock::time_point lastTime;

	void mainLoop();
};

