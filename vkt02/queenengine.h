#pragma once

#include "qeheader.h"

class QueenEngine
{
public:
	QueenEngine(QeGlobalKey& _key) {}
	~QueenEngine() {}
	void run();

	QeActivity* activity;

	int currentFPS;
	std::chrono::steady_clock::time_point lastTime;

	void mainLoop();
	void cleanup();
};

