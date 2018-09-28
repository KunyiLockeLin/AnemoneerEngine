#include "qeheader.h"


void QeBillboard::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
	AST->setGraphicsShader(graphicsShader, nullptr, "billboard");

	AST->getXMLiValue(&materialOID, initProperty, 1, "materialOID");

	bUpdateMaterialOID = false;
	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);
	bRotate = false;
}