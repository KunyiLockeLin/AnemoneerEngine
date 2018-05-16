#include "qeheader.h"


void QueenEngine::run() {
	
	bClosed = false;
	bRestart = false;
;
	DEBUG->init();
	WIN->init();
	VK->init();

	QeAssetXML* node = AST->getXML(AST->CONFIG);

	if(activityName.empty())
		activityName = AST->getXMLValue(node, 1, "startScene");
	
	QeAssetXML* startActivityNode = AST->getXMLNode(node, 1, activityName.c_str());
	currentActivity = OBJMGR->getActivity(0, startActivityNode);
	renderFPSTimer.setTimer(1000 / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "renderFPS")));
	computeFPSTimer.setTimer(1000 / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "computeFPS")));

	mainLoop();
}


void QueenEngine::mainLoop() {
	while (!bClosed && !bRestart) {

		int passMilliSecond;
		if (renderFPSTimer.checkTimer(passMilliSecond)) {

			currentRenderFPS = 1000 / passMilliSecond;
			float time = float(passMilliSecond) / 1000;
			currentActivity->updateRender(time);
			VP->updateRender(time);
			VK->updateRender(time);
			WIN->updateRender(time);
		}
		if (computeFPSTimer.checkTimer(passMilliSecond)) {

			currentComputeFPS = 1000 / passMilliSecond;
			float time = float(passMilliSecond) / 1000;
			currentActivity->updateCompute(time);
			VP->updateCompute(time);
			VK->updateCompute(time);
			WIN->updateCompute(time);
		}
	}
	vkDeviceWaitIdle(VK->device);
}