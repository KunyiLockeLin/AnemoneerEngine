#include "qeheader.h"


void QeLine::init(QeAssetXML* _property, int _parentOID) {
	
	QePoint::init(_property, _parentOID);

	if(!strcmp(initProperty->key.c_str(), "e"))  modelData = AST->getModel("line");
	else										 modelData = AST->getModel(initProperty->key.c_str());
	
	//mtlData = AST->getMaterialImage("");
	//bufferData.material = mtlData->value;
	AST->setGraphicsShader(graphicsShader, editProperty,"line");
}

QeDataDescriptorSetModel QeLine::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	return descriptorSetData;
}

void QeLine::updateCompute(float time) {
	updateBuffer();
}

void QeLine::updateRender(float time) {
}

void QeLine::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
}