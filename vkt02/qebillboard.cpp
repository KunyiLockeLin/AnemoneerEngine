#include "qeheader.h"


void QeBillboard::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	mtlData = AST->getMaterialImage(AST->getXMLValue(editProperty, 1, "image"));
	AST->getXMLfValue(&mtlData->value.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	bufferData.material = mtlData->value;
	AST->setGraphicsShader(graphicsShader, editProperty, "billboard");
}

QeDataDescriptorSetModel QeBillboard::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageView = mtlData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSampler = mtlData->image.pBaseColorMap->sampler;
	return descriptorSetData;
}