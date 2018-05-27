#pragma once

#include "qeheader.h"

class QeQueenEngineKey
{
	friend class QueenEngine;

private:
	QeQueenEngineKey() {}
};

class QueenEngine
{
private:
	QeQueenEngineKey key;

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

