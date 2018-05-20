#include "qeheader.h"


void QeCube::init(QeAssetXML* _property) {

	initProperty = _property;

	modelData = AST->getModel("cube", true);
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"), true);
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	data.cubeMapImageViews = pMaterial->pCubeMap->buffer.view;
	data.cubeMapSamplers = pMaterial->pCubeMap->sampler;
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

	const char * c = AST->getXMLValue(_property, 1, "shadervert");
	if (c == nullptr) {
		c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "cubevert");
		if (c == nullptr) {
			if (modelData->animationNum == 0)
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "vert");
			else
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "skeletonvert");
		}
	}
	if (c != nullptr)	pMaterial->pShaderVert = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shadergeom");
	if (c == nullptr) {
		c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "cubegeom");
		if (c == nullptr) {
			if (pMaterial->type == eMaterialPhong)
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "phonggeom");
			else if (pMaterial->type == eMaterialPBR)
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrgeom");
		}
	}
	if (c != nullptr)	pMaterial->pShaderGeom = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shaderfrag");
	if (c == nullptr) {
		c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "cubefrag");
		if (c == nullptr) {
			if (pMaterial->type == eMaterialPhong)
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "phongfrag");
			else if (pMaterial->type == eMaterialPBR)
				c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrfrag");
		}
	}
	if (c != nullptr)	pMaterial->pShaderFrag = AST->getShader(c);

	createPipeline();

	c = AST->getXMLValue(_property, 1, "id");
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
