#include "qeheader.h"


void QeRender::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	AST->setGraphicsShader(graphicsShader, editProperty, "render");
	QeDataRender* render = VP->getSubRender(cameraOID);
}

QeDataDescriptorSetModel QeRender::createDescriptorSetModel() {

	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	QeDataRender* render = VP->getSubRender(cameraOID);

	descriptorSetData.baseColorMapImageView = render->presentImage.view;
	descriptorSetData.baseColorMapSampler = render->presentImage.sampler;
	return descriptorSetData;
}