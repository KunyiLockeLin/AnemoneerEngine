#include "qeheader.h"


void QeBillboard::init(QeAssetXML* _property) {

	initProperty = _property;

	//modelData = AST->getModel("point");
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"));
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	//VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);
	VK->createBuffer(uboBuffer, sizeof(QeUniformBufferObject), nullptr);

	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	data.materialBuffer = pMaterial->uboBuffer.buffer;
	data.diffuseMapImageViews = pMaterial->image.pDiffuseMap->view;
	data.diffueMapSamplers = pMaterial->image.pDiffuseMap->sampler;
	VK->updateDescriptorSet(data, descriptorSet);

	pos = { 0, 0, 0 };
	face = 0.0f;
	up = 0.0f;
	size = {1,1,1};
	speed = 0;
	currentActionID = 0;
	actionType = eActionTypeOnce;
	actionState = eActionStateStop;
	attachID = 0;
	attachSkeletonName = nullptr;
	actionSpeed = 0;

	AST->setShader(pMaterial->shader, _property, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "billboard"));

	const char * c = AST->getXMLValue(_property, 1, "alpha");
	if (c != nullptr)	bAlpha = atoi(c);

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

	c = AST->getXMLValue(_property, 1, "show");
	if (c != nullptr)	bShow = atoi(c);

	c = AST->getXMLValue(_property, 1, "culling");
	if (c != nullptr)	cullingDistance = atoi(c);

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

void QeBillboard::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, eGraphicsPipeLine_Point, bAlpha);
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

void QeBillboard::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;

	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, 1, 1, 0, 0);
}