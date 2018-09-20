#include "qeheader.h"


void QeAxis::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	modelData = AST->getModel("axis");

	AST->setGraphicsShader(graphicsShader, nullptr, "line");

	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);
}

void QeAxis::update1() {
	bufferData.model = owner->transform->worldTransformMatrix(true, true);
	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}