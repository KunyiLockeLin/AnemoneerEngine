#include "qeheader.h"


void QeLine::init(QeAssetXML* _property) {
	initProperty = _property;

	if( lineType.length() == 0 ) modelData = AST->getModel("line");
	else					  modelData = AST->getModel(lineType.c_str());
	pMaterial = AST->getMaterialImage("");

	AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "line"));

	setProperty();
}

QeDataDescriptorSetModel QeLine::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	return descriptorSetData;
}

void QeLine::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, eGraphicsPipeLine_Point);
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

	bufferData.model = mat;
}

void QeLine::updateCompute(float time) {
	updateBuffer();
}

void QeLine::updateRender(float time) {
}

void QeLine::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets();
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
}