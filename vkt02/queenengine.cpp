#include "qeheader.h"


void QueenEngine::run() {
	
	bClosed = false;
	bRestart = false;

	QeAssetXML* node = AST->getXML(AST->CONFIG);

	GLB.libGLFW = atoi(AST->getXMLValue(node, 1, "glfw"));
	GLB.libGLM = atoi(AST->getXMLValue(node, 1, "glm"));
	GLB.libGLI = atoi(AST->getXMLValue(node, 1, "gli"));
	
	DEBUG->init();
	WIN->init();
	VK->init();

	if(activityName.empty())
		activityName = AST->getXMLValue(node, 1, "startScene");
	
	QeAssetXML* startActivityNode = AST->getXMLNode(node, 1, activityName.c_str());
	if (currentActivity) delete currentActivity;
	currentActivity = new QeActivity(key);
	currentActivity->init(startActivityNode);
	renderFPSTimer.setTimer(1000 / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "renderFPS")));
	computeFPSTimer.setTimer(1000 / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "computeFPS")));

	mainLoop();
}

void QueenEngine::mainLoop() {
	while (!bClosed && !bRestart) {

		int passMilliSecond;
		if (computeFPSTimer.checkTimer(passMilliSecond)) {

			currentComputeFPS = 1000 / passMilliSecond;
			float time = float(passMilliSecond) / 1000;
			currentActivity->updateCompute(time);
			VP->updateCompute(time);
			VK->updateCompute(time);
			WIN->updateCompute(time);
		}

		if (renderFPSTimer.checkTimer(passMilliSecond)) {

			currentRenderFPS = 1000 / passMilliSecond;
			float time = float(passMilliSecond) / 1000;
			currentActivity->updateRender(time);
			VP->updateRender(time);
			VK->updateRender(time);
			WIN->updateRender(time);
		}
	}
	vkDeviceWaitIdle(VK->device);
}