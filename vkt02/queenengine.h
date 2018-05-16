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
	int currentRenderFPS = 0;
	int currentComputeFPS = 0;
	QeTimer renderFPSTimer;
	QeTimer computeFPSTimer;

	void mainLoop();
};

