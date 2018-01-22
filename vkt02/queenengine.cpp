#include "qeheader.h"


void QueenEngine::run() {
	bClosed = false;
	bRestart = false;
	lastTime = std::chrono::high_resolution_clock::now();
	DEBUG->init();
	WIN->init();
	VK->init();

	QeAssetXML* node = AST->getXML(AST->CONFIG);

	if(activityName.empty())
		activityName = AST->getXMLValue(node, 1, "startActivity");
	
	QeAssetXML* startActivityNode = AST->getXMLNode(node, 1, activityName.c_str());
	currentActivity = OBJMGR->getActivity(0, startActivityNode);

	mainLoop();
}


void QueenEngine::mainLoop() {
	while (!bClosed && !bRestart) {

		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;

		if (time >= 1.0f / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "FPS"))) {

			lastTime = currentTime;
			currentFPS = int(1 / time);
			
			currentActivity->update(time);
			VP->update(time);
			VK->update(time);
			WIN->update(time);
		}
	}
	VK->deviceWaitIdle();
}