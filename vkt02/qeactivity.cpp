#include "qeheader.h"


void QeActivity::init(QeAssetXML* _property) {
	initProperty = _property;
	name = _property->key.c_str();

	QeAssetXML* node = AST->getXMLNode(_property, 1, "ambientColor");
	if (node == nullptr || node->eKeys.size() == 0)	node = AST->getXMLNode(2, AST->CONFIG, "defaultAmbientColor");

	ambientColor = { float(atof(AST->getXMLValue(node, 1, "r"))),
		float(atof(AST->getXMLValue(node, 1, "g"))), float(atof(AST->getXMLValue(node, 1, "b"))), 1.0f };

	node = AST->getXMLNode(_property, 1, "cameras");
	if (node == nullptr || node->nexts.size() == 0) {
		node = AST->getXMLNode(2, AST->CONFIG, "defaultCamera");
		VP->init(node);
	}
	else	VP->init(node->nexts[0]);

	node = AST->getXMLNode(_property, 1, "lights");
	if (node == nullptr || node->nexts.size() == 0) {
		node = AST->getXMLNode(2, AST->CONFIG, "defaultLight");	
		OBJMGR->getLight(0, node);
	}
	else	for ( int index = 0; index < node->nexts.size(); ++index)	OBJMGR->getLight(index, node->nexts[index]);

	node = AST->getXMLNode(_property, 1, "models");
	if (node != nullptr && node->nexts.size() > 0) 
		for (int index = 0; index < node->nexts.size(); ++index)	OBJMGR->getModel(0, node->nexts[index]);

	node = AST->getXMLNode(_property, 1, "postShader");
	if (node == nullptr || node->eKeys.size() == 0) node = AST->getXMLNode(2, AST->CONFIG, "defaultPostprocessing");
	if (node != nullptr && node->eKeys.size() > 0) {

		const char* c = AST->getXMLValue(node, 1, "vert");
		if (c != nullptr) VK->pPostProcessingVert = AST->getShader(c);
		c = AST->getXMLValue(node, 1, "geom");
		if (c != nullptr) VK->pPostProcessingGeom = AST->getShader(c);
		c = AST->getXMLValue(node, 1, "frag");
		if (c != nullptr) VK->pPostProcessingFrag = AST->getShader(c);
	}
	VK->bUpdateDrawCommandBuffers = true;
	VK->updatePostProcessing();
}

void QeActivity::eventInput(int _input1, int _input2, int _param1, int _param2) {
	
	switch (_input1) {

	case WM_KEYDOWN:
		switch (_input2) {
		case VK_ESCAPE:
			QE->bClosed = true;
			break;
		case KEY_L:
			QE->bRestart = true;
			break;
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
			VP->setTargetCamera(_input2 - VK_NUMPAD0);
			break;
		case VK_ADD:
			VP->addNewViewport();
			break;
		case VK_SUBTRACT:
			VP->popViewport();
			break;
		case KEY_W:
		case VK_UP:
			VP->getTargetCamera()->move(QeVector3f(0, 0, 1));
			break;
		case KEY_S:
		case VK_DOWN:
			VP->getTargetCamera()->move(QeVector3f(0, 0, -1));
			break;
		case KEY_D:
		case VK_RIGHT:
			VP->getTargetCamera()->move(QeVector3f(1, 0, 0));
			break;
		case KEY_A:
		case VK_LEFT:
			VP->getTargetCamera()->move(QeVector3f(-1, 0, 0));
			break;
		case KEY_Q:
			VP->getTargetCamera()->move(QeVector3f(0, 1, 0));
			break;
		case KEY_E:
			VP->getTargetCamera()->move(QeVector3f(0, -1, 0));
			break;
		case KEY_R:
			VP->getTargetCamera()->reset();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		VP->getTargetCamera()->setMousePos(QeVector2i(_param1, _param2));
		break;
	case WM_MOUSEMOVE:

		switch (_input2) {
		case MK_LBUTTON:
			VP->getTargetCamera()->rotateTarget(QeVector2i(_param1, _param2));
			break;
		case MK_RBUTTON:
			VP->getTargetCamera()->rotatePos(QeVector2i(_param1, _param2));
			break;
		}
		break;
	}
}

void QeActivity::update(float time) {
	OBJMGR->update(time);
}