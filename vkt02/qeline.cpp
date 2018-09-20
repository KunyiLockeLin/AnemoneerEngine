#include "qeheader.h"


void QeLine::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeComponent::initialize(_property, _owner);

	AST->getXMLiValue(&targetTransformOID, initProperty, 1, "targetTransformOID");

	AST->getXMLfValue(&color.x, initProperty, 1, "colorR");
	AST->getXMLfValue(&color.y, initProperty, 1, "colorG");
	AST->getXMLfValue(&color.z, initProperty, 1, "colorB");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	modelData = AST->getModel("line", false, (float*)&color);

	AST->setGraphicsShader(graphicsShader, nullptr, "line");

	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);
}

void QeLine::update1() {

	if (targetTransformOID == 0) return;
	
	QeTransform* targetTransform = (QeTransform*)OBJMGR->findComponent(eComponent_transform, targetTransformOID);
	if (targetTransform == 0) return;
	
	QeVector3f targetPos = targetTransform->worldPosition();
	QeVector3f pos = owner->transform->worldPosition();

	QeVector3f vec = targetPos - pos;
	float size = MATH->fastSqrt(MATH->length(vec));
	QeVector3f scale = { size, size ,size };
	bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), MATH->vectorToEulerAngles(vec), scale);

	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}

QeDataDescriptorSetModel QeLine::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	return descriptorSetData;
}