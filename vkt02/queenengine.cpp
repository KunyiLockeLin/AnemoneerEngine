#include "qeheader.h"


void QueenEngine::run() {
	lastTime = std::chrono::high_resolution_clock::now();

	VLK->init();

	QeAssetXML* node = AST->getXML(AST->CONFIG);
	const char* initActivityName = AST->getXMLValue(node, 1, "initActivity");
	QeAssetXML* initActivityNode = AST->getXMLNode(node, 1, initActivityName);

	currentActivity = OBJMGR->getActivity(0, initActivityNode);

	mainLoop();
}


void QueenEngine::mainLoop() {
	while (!WIN->isWinodowShouldClose()) {

		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;

		if (time >= 1.0f / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "FPS"))) {

			lastTime = currentTime;
			currentFPS = int(1 / time);
			
			currentActivity->update(time);
			VP->update(time);
			VLK->update(time);
			WIN->update(time);
		}
	}
	VLK->deviceWaitIdle();
}