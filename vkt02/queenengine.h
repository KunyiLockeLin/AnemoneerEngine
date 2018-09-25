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
	QueenEngine(QeGlobalKey& _key):sceneEID(0){}
	//~QueenEngine() {}
	void run();

	int sceneEID;
	bool bPause = false;
	bool bClosed = false;
	bool bRestart = false;
	float deltaTime;
	int currentFPS = 0;
	QeTimer FPSTimer;

	void mainLoop();
};