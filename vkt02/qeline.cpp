#include "qeheader.h"


void QeLine::init(QeAssetXML* _property, int _parentOID) {
	
	QePoint::init(_property, _parentOID);

	if(!strcmp(initProperty->key.c_str(), "e"))  modelData = AST->getModel("line");
	else										 modelData = AST->getModel(initProperty->key.c_str());
	
	mtlData = AST->getMaterialImage("");
	AST->getXMLfValue(&mtlData->value.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	bufferData.material = mtlData->value;
	AST->setGraphicsShader(graphicsShader, editProperty,"line");
}

QeDataDescriptorSetModel QeLine::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	return descriptorSetData;
}

void QeLine::updateCompute(float time) {
	updateBuffer();
}

void QeLine::updateRender(float time) {
}