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
	QueenEngine(QeGlobalKey& _key){}
	//~QueenEngine() {}
	void run();

	bool bPause = false;
	bool bClosed = false;
	bool bRestart = false;
	float deltaTime;
	int currentFPS = 0;
	QeTimer FPSTimer;

	void mainLoop();
};