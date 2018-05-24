#include "qeheader.h"


void QeCube::init(QeAssetXML* _property) {

	initProperty = _property;

	modelData = AST->getModel("cube", true);
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"), true);
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	data.cubeMapImageViews = pMaterial->image.pCubeMap->view;
	data.cubeMapSamplers = pMaterial->image.pCubeMap->sampler;
	VK->updateDescriptorSet(data, descriptorSet);

	pos = { 0, 0, 0 };
	face = 0.0f;
	up = 0.0f;
	size = modelData->scale;
	speed = 0;
	currentActionID = 0;
	actionType = eActionTypeOnce;
	actionState = eActionStateStop;
	attachID = 0;
	cubeMapID = 0;
	attachSkeletonName = nullptr;
	actionSpeed = 0;

	AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "cubemap"));
	createGraphicsPipeline();

	const char * c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	pos.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	pos.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "scaleX");
	if (c != nullptr)	size.x *= float(atof(c));
	c = AST->getXMLValue(_property, 1, "scaleY");
	if (c != nullptr)	size.y *= float(atof(c));
	c = AST->getXMLValue(_property, 1, "scaleZ");
	if (c != nullptr)	size.z *= float(atof(c));
}
