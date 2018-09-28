#include "qeheader.h"


void QeBillboard::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);
	const char * image = AST->getXMLValue(initProperty, 1, "image");
	materialData = AST->getMaterialImage(image);
	materialData->value.baseColor = { 1,1,1,1 };
	AST->getXMLfValue(&materialData->value.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
	bufferData.material = materialData->value;
	AST->setGraphicsShader(graphicsShader, nullptr, "billboard");

	AST->getXMLbValue(&graphicsPipeline.bAlpha, initProperty, 1, "alpha");

	if (graphicsPipeline.bAlpha)	GRAP->alphaModels.push_back(this);
	else							GRAP->models.push_back(this);
	bRotate = false;
}

QeDataDescriptorSetModel QeBillboard::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageView = materialData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSampler = materialData->image.pBaseColorMap->sampler;
	return descriptorSetData;
}