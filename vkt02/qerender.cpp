#include "qeheader.h"


void QeRender::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	//modelData = AST->getModel("plane");
	AST->setGraphicsShader(graphicsShader, editProperty, "shadow");
	bufferData.material.baseColor = {1,1,1,1};

	QeDataRender* render = VP->getRender(eRender_shadow, cameraOID);
}

QeDataDescriptorSetModel QeRender::createDescriptorSetModel() {

	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	QeDataRender* render = VP->getRender(eRender_shadow, cameraOID);

	//descriptorSetData.baseColorMapImageView = render->colorImage.view;
	//descriptorSetData.baseColorMapSampler = render->colorImage.sampler;
	descriptorSetData.shadowMapImageView = render->depthImage.view;
	descriptorSetData.shadowMapSampler = render->depthImage.sampler;
	return descriptorSetData;
}