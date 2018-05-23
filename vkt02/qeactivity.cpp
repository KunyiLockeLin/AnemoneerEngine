#include "qeheader.h"


void QeActivity::init(QeAssetXML* _property) {
	initProperty = _property;
	name = _property->key.c_str();

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
		OBJMGR->getLight(0, node);
	}
	else	for ( int index = 0; index < node->nexts.size(); ++index)	OBJMGR->getLight(index, node->nexts[index]);

	node = AST->getXMLNode(_property, 1, "cubemaps");
	if (node != nullptr && node->nexts.size() > 0)
		for (int index = 0; index < node->nexts.size(); ++index)	OBJMGR->getCube(0, node->nexts[index]);

	node = AST->getXMLNode(_property, 1, "models");
	if (node != nullptr && node->nexts.size() > 0) 
		for (int index = 0; index < node->nexts.size(); ++index)	OBJMGR->getModel(0, node->nexts[index]);

	axis = OBJMGR->getLine(0, initProperty, "axis");
	VP->getTargetCamera()->updateAxis(axis);

	grids = OBJMGR->getLine(1, initProperty, "grids");
}

void QeActivity::eventInput(QeInputData & inputData) {
	VP->updateInput(inputData);
}

void QeActivity::updateRender(float time) {
	OBJMGR->updateRender(time);
}

void QeActivity::updateCompute(float time) {
	OBJMGR->updateCompute(time);
}