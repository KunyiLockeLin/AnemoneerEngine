#include "qeheader.h"


void QeRender::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	AST->setGraphicsShader(graphicsShader, editProperty, "render");
	bufferData.material.baseColor = {1,1,1,1};

	QeDataRender* render = VP->getSubRender(cameraOID);
}

QeDataDescriptorSetModel QeRender::createDescriptorSetModel() {

	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	QeDataRender* render = VP->getSubRender(cameraOID);

	descriptorSetData.baseColorMapImageView = render->attachImage.view;
	descriptorSetData.baseColorMapSampler = render->attachImage.sampler;
	return descriptorSetData;
}