#include "qeheader.h"


void QeLine::init(QeAssetXML* _property) {
	initProperty = _property;

	if( lineType.length() == 0 ) modelData = AST->getModel("line");
	else					  modelData = AST->getModel(lineType.c_str());
	pMaterial = AST->getMaterialImage("");

	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	//VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);
	VK->createBuffer(uboBuffer, sizeof(QeUniformBufferObject), nullptr);

	QeDataDescriptorSet data;
	data.uboBuffer = uboBuffer.buffer;
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

	AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "line"));

	createPipeline();

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
	else if (modelData->rootJoint) {
		actionPlay();
		updateAction(0);
		actionStop();
	}

	c = AST->getXMLValue(_property, 1, "attachid");
	if (c != nullptr)	attachID = atoi(c);

	attachSkeletonName = AST->getXMLValue(_property, 1, "attachskeleton");
}
void QeLine::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, ePipeLine_Point);
}

void QeLine::setMatModel() {

	QeMatrix4x4f mat;

	mat *= MATH->translate(pos);
	mat *= MATH->rotateY(up);
	//mat *= MATH->rotate(up, QeVector3f(0.0f, 1.0f, 0.0f));
	mat *= MATH->rotateZ(face);
	//mat *= MATH->rotate(face, QeVector3f(0.0f, 0.0f, 1.0f));
	float dis = MATH->distance( VP->getTargetCamera()->pos, pos )/10;
	dis = dis < 0.1f ? 0.1f : dis;
	//dis = 1;
	mat *= MATH->scale(size*dis);

	ubo.model = mat;
}

void QeLine::updateRender(float time) {
	updateUniformBuffer();
}

void QeLine::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
}