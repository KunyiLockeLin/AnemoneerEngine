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
	QeActivity* activity = nullptr;
	bool bClosed = false;
	bool bRestart = false;
	int currentRenderFPS = 0;
	int currentComputeFPS = 0;
	QeTimer renderFPSTimer;
	QeTimer computeFPSTimer;

	void mainLoop();
};

