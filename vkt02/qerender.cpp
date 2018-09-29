#include "qeheader.h"


void QeRender::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);
	//modelData = AST->getModel("plane");

	shaderKey = "render";
	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

	AST->getXMLiValue((int*)&renderSize.width, initProperty, 1, "width");
	AST->getXMLiValue((int*)&renderSize.height, initProperty, 1, "height");
	AST->getXMLiValue(&targetCameraOID, initProperty, 1, "targetCameraOID");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);

	GRAP->getRender(eRender_color, targetCameraOID, renderSize);
}

QeDataDescriptorSetModel QeRender::createDescriptorSetModel() {

	descriptorSet.bRender = true;
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	QeDataRender* render = GRAP->getRender(eRender_color, targetCameraOID, renderSize);

	descriptorSetData.baseColorMapImageView = render->colorImage.view;
	descriptorSetData.baseColorMapSampler = render->colorImage.sampler;
	return descriptorSetData;
}


void QeRender::update1() {

	QeVector3f scale = owner->transform->worldScale();
	scale.x *= MATH->fastSqrt((float(renderSize.width) / renderSize.height));
	bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(), scale);

	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}