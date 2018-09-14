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
	if (scene) delete scene;

	scene = new QeScene(key);
	scene->init(node);
	FPSTimer.setTimer(1000 / std::stoi(AST->getXMLValue(3, AST->CONFIG, "envir", "FPS")));

	mainLoop();
}

void QueenEngine::mainLoop() {
	while (!bClosed && !bRestart) {

		int passMilliSecond;
		if (FPSTimer.checkTimer(passMilliSecond)) {

			currentFPS = 1000 / passMilliSecond;
			deltaTime = float(passMilliSecond) / 1000;

			VK->update1();
			VP->update1();
			WIN->update1();
			SCENE->update1();
			OBJMGR->update1();

			WIN->update2();
			SCENE->update2();
			OBJMGR->update2();
			VP->update2();
			VK->update2();
		}
	}
	vkDeviceWaitIdle(VK->device);
}