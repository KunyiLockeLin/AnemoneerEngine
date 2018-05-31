#include "qeheader.h"


void QeActivity::init(QeAssetXML* _property) {
	initProperty = _property;
	name = _property->key.c_str();

	const char * c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	QeAssetXML*node = AST->getXMLNode(_property, 1, "cameras");
	if (node == nullptr || node->nexts.size() == 0) {
		node = AST->getXMLNode(2, AST->CONFIG, "defaultCamera");
		VP->init(node);
	}
	else	VP->init(node->nexts[0]);

	AST->setShader(VP->shader, AST->getXMLNode(_property, 1, "postprocessing"), AST->getXMLNode(3, AST->CONFIG, "defaultShader", "postprocessing"));

	VP->updatePostProcessing();
	VP->bUpdateDrawCommandBuffers = true;

	node = AST->getXMLNode(_property, 1, "ambientColor");
	if (node == nullptr || node->eKeys.size() == 0)	node = AST->getXMLNode(2, AST->CONFIG, "defaultAmbientColor");

	ambientColor = { float(atof(AST->getXMLValue(node, 1, "r"))),
		float(atof(AST->getXMLValue(node, 1, "g"))), float(atof(AST->getXMLValue(node, 1, "b"))), 1.0f };

	node = AST->getXMLNode(_property, 1, "lights");
	if (node == nullptr || node->nexts.size() == 0) {
		node = AST->getXMLNode(2, AST->CONFIG, "defaultLight");
		uint16_t id = atoi(AST->getXMLValue(node, 1, "id"));
		light = OBJMGR->getLight(id, node);
	}
	else{
		for (int index = 0; index < node->nexts.size(); ++index) {
			uint16_t id = atoi(AST->getXMLValue(node->nexts[index], 1, "id"));
			light = OBJMGR->getLight(id, node->nexts[index]);
		}
	}

	node = AST->getXMLNode(_property, 1, "cubemaps");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			uint16_t id = atoi(AST->getXMLValue(node->nexts[index], 1, "id"));
			OBJMGR->getCube(id, node->nexts[index]);
		}
	}
	node = AST->getXMLNode(_property, 1, "models");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			uint16_t id = atoi(AST->getXMLValue(node->nexts[index], 1, "id"));
			OBJMGR->getModel(id, node->nexts[index]);
		}
	}
	node = AST->getXMLNode(_property, 1, "points");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			uint16_t id = atoi(AST->getXMLValue(node->nexts[index], 1, "id"));
			OBJMGR->getPoint(id, node->nexts[index]);
		}
	}
	axis = OBJMGR->getLine(1, initProperty, "axis");
	//VP->getTargetCamera()->updateAxis();
	grids = OBJMGR->getLine(2, initProperty, "grids");
}

void QeActivity::eventInput(QeInputData & inputData) {
	
	// viewport
	switch (inputData.inputType) {

	case WM_KEYDOWN:
		switch (inputData.inputKey) {

		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
			VP->setTargetCamera(inputData.inputKey - VK_NUMPAD0);
			break;
		case VK_ADD:
			VP->addNewViewport();
			break;
		case VK_SUBTRACT:
			VP->popViewport();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	// main camera
	switch (inputData.inputType) {

	case WM_KEYDOWN:
		switch (inputData.inputKey) {

		case VK_UP:
			VP->getTargetCamera()->rotateTarget(-10, QeVector3f(0, 1, 0));
			break;
		case VK_DOWN:
			VP->getTargetCamera()->rotateTarget(10, QeVector3f(0, 1, 0));
			break;
		case VK_RIGHT:
			VP->getTargetCamera()->rotateTarget(-10, QeVector3f(0, 0, 1));
			break;
		case VK_LEFT:
			VP->getTargetCamera()->rotateTarget(10, QeVector3f(0, 0, 1));
			break;
		case KEY_E:
			VP->getTargetCamera()->move(QeVector3f(0, 1, 0));
			break;
		case KEY_C:
			VP->getTargetCamera()->move(QeVector3f(0, -1, 0));
			break;
		case KEY_A:
			VP->getTargetCamera()->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_D:
			VP->getTargetCamera()->move(QeVector3f(1, 0, 0));
			break;
		case KEY_W:
			VP->getTargetCamera()->move(QeVector3f(0, 0, 1));
			break;
		case KEY_S:
			VP->getTargetCamera()->move(QeVector3f(0, 0, -1));
			break;
		case KEY_Q:
			VP->getTargetCamera()->move(QeVector3f(0, 0, 1), false);
			break;
		case KEY_Z:
			VP->getTargetCamera()->move(QeVector3f(0, 0, -1), false);
			break;
		case KEY_X:
			VP->getTargetCamera()->reset();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		VP->getTargetCamera()->setMousePos(inputData.mousePos);
		break;
	case WM_MOUSEMOVE:

		switch (inputData.inputKey) {
		case MK_LBUTTON:
			VP->getTargetCamera()->rotateTarget(inputData.mousePos);
			break;
		case MK_RBUTTON:
			//rotatePos(inputData.mousePos);
			VP->getTargetCamera()->zoomInOut(inputData.mousePos);
			break;
		}
		break;
	}
}

void QeActivity::updateRender(float time) {
	OBJMGR->updateRender(time);
}

void QeActivity::updateCompute(float time) {
	OBJMGR->updateCompute(time);
}