#include "qeheader.h"


void QeActivity::init(QeAssetXML* _property) {
	initProperty = _property;

	QeAssetXML * node;
	AST->getXMLiValue(&eid, initProperty, 1, "eid");

	node = AST->getXMLNode(_property, 1, "ambientColor");
	if (!node)	node = AST->getXMLNode(3, AST->CONFIG, "default","ambientColor");

	if (node) {
		AST->getXMLfValue(&ambientColor.x, node, 1, "r");
		AST->getXMLfValue(&ambientColor.y, node, 1, "g");
		AST->getXMLfValue(&ambientColor.z, node, 1, "b");
		ambientColor.w = 1.0f;
	}

	VP->init(initProperty);

	node = AST->getXMLNode(initProperty, 1, "lights");
	if (!node)	node = AST->getXMLNode(3, AST->CONFIG, "default", "lights");
	if (node){
		size_t size = node->nexts.size();
		for (int index = 0; index < size; ++index) {
			light = (QeLight*)OBJMGR->getObject(0, node->nexts[index]);
		}
	}

	node = AST->getXMLNode(initProperty, 1, "cubemaps");
	if (node) {
		size_t size = node->nexts.size();
		for (int index = 0; index < size; ++index) {
			OBJMGR->getObject(0, node->nexts[index]);
		}
	}

	node = AST->getXMLNode(initProperty, 1, "models");
	if (node) {
		size_t size = node->nexts.size();
		for (int index = 0; index < size; ++index) {
			OBJMGR->getObject(0, node->nexts[index]);
		}
	}

	node = AST->getXMLNode(initProperty, 1, "points");
	if (node) {
		size_t size = node->nexts.size();
		for (int index = 0; index < size; ++index) {
			OBJMGR->getObject(0, node->nexts[index]);
		}
	}

	gridsNum = 0;
	node = AST->getXMLNode(_property, 1, "grids");
	if (!node)	node = AST->getXMLNode(3, AST->CONFIG, "default", "grids");
	if (node) {
		AST->getXMLiValue(&gridsNum, node, 1, "num");
		grids = (QeLine*)OBJMGR->getObject(0, node);
	}

	node = AST->getXMLNode(_property, 1, "axis");
	if (!node)	node = AST->getXMLNode(3, AST->CONFIG, "default", "axis");
	if (node)	axis = (QeLine*)OBJMGR->getObject(0, node);
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
			VP->setTargetCamera(inputData.inputKey - VK_NUMPAD1);
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
			VP->getTargetCamera()->rotateTarget(-10, { 1, 0 });
			break;
		case VK_DOWN:
			VP->getTargetCamera()->rotateTarget(10, { 1, 0 });
			break;
		case VK_RIGHT:
			VP->getTargetCamera()->rotateTarget(10, { 0, 1 });
			break;
		case VK_LEFT:
			VP->getTargetCamera()->rotateTarget(-10, { 0, 1 });
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

void QeActivity::updateCompute(float time) {}
void QeActivity::updateRender(float time) {}