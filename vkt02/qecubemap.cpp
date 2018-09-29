#include "qeheader.h"


void QeCubemap::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeComponent::initialize(_property, _owner);

	modelData = AST->getModel("cube", true);
	const char * image = AST->getXMLValue(initProperty, 1, "image");
	materialData = AST->getMaterialImage(image, true);

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
	bufferData.material = materialData->value;
	shaderKey = "cubemap";
	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);
	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);
}

QeDataDescriptorSetModel QeCubemap::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.cubeMapImageView = materialData->image.pCubeMap->view;
	descriptorSetData.cubeMapSampler = materialData->image.pCubeMap->sampler;
	return descriptorSetData;
}