#include "qeheader.h"


void QueenEngine::run() {
	
	bClosed = false;
	bRestart = false;

	QeAssetXML* node = AST->getXML(AST->CONFIG);

	//GLB.libGLFW = atoi(AST->getXMLValue(node, 1, "glfw"));
	//GLB.libGLM = atoi(AST->getXMLValue(node, 1, "glm"));
	//GLB.libGLI = atoi(AST->getXMLValue(node, 1, "gli"));
	
	DEBUG->init();
	WIN->init();
	VK->init();

	if(sceneEID ==0)	AST->getXMLiValue(&sceneEID, node, 1, "startScene");
	
	node = AST->getXMLEditNode(eObject_Scene, sceneEID);
	if (activity) delete activity;

	activity = new QeActivity(key);
	activity->init(node);
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
			VK->updateCompute(time);
			VP->updateCompute(time);
			WIN->updateCompute(time);
			ACT->updateCompute(time);
			OBJMGR->updateCompute(time);
		}

		if (renderFPSTimer.checkTimer(passMilliSecond)) {

			currentRenderFPS = 1000 / passMilliSecond;
			float time = float(passMilliSecond) / 1000;
			WIN->updateRender(time);
			ACT->updateRender(time);
			OBJMGR->updateRender(time);
			VP->updateRender(time);
			VK->updateRender(time);
		}
	}
	vkDeviceWaitIdle(VK->device);
}