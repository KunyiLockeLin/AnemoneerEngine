#include "qeheader.h"


void QeBillboard::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	modelData = nullptr;
	materialData = nullptr;
	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	shaderKey = "billboard";
	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

	AST->getXMLiValue(&materialOID, initProperty, 1, "materialOID");

	bUpdateMaterialOID = false;
	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);
	bRotate = false;
	computeShader = VK_NULL_HANDLE;
	computePipeline = VK_NULL_HANDLE;
}