#include "qeheader.h"



void QeBillboard::init(QeAssetXML* _property) {

	initProperty = _property;

	modelData = AST->getModel("plane");
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"));
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	data.materialBuffer = pMaterial->uboBuffer.buffer;
	data.diffuseMapImageViews = pMaterial->image.pDiffuseMap->view;
	data.diffueMapSamplers = pMaterial->image.pDiffuseMap->sampler;

	cubeMapID = 0;
	const char * c = AST->getXMLValue(_property, 1, "cubemapid");
	if (c != nullptr)	cubeMapID = atoi(c);

	if (cubeMapID > 0) {
		QeModel* model = OBJMGR->getCube(cubeMapID, nullptr);
		if (model != nullptr) {
			data.cubeMapImageViews = model->pMaterial->image.pCubeMap->view;
			data.cubeMapSamplers = model->pMaterial->image.pCubeMap->sampler;
		}
	}
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
	attachSkeletonName = nullptr;
	actionSpeed = 0;

	AST->setShader(pMaterial->shader, _property, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "billboard"));

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

	c = AST->getXMLValue(_property, 1, "speed");
	if (c != nullptr)	speed = atoi(c);

	c = AST->getXMLValue(_property, 1, "actionSpeed");
	if (c != nullptr)	actionSpeed = float(atof(c));

	c = AST->getXMLValue(_property, 1, "action");
	if (c != nullptr) {
		currentActionID = atoi(c);

		c = AST->getXMLValue(_property, 1, "actionType");
		if (c != nullptr)	actionType = QeActionType(atoi(c));

		setAction(currentActionID, actionType);
		actionPlay();
	}

	c = AST->getXMLValue(_property, 1, "attachid");
	if (c != nullptr)	attachID = atoi(c);

	attachSkeletonName = AST->getXMLValue(_property, 1, "attachskeleton");
}

void QeBillboard::setMatModel() {
	
	//type = eBillboardFaceAndSize;

	QeMatrix4x4f mat;
	mat *= MATH->translate(pos);
	//size = { 3,3,3 };
	mat *= MATH->scale(size);

	ubo.model = mat;
	ubo.param.y = float(type);
}

//void QeBillboard::updateRender(float time) { updateUniformBuffer(); }
//void QeBillboard::updateCompute(float time) {}