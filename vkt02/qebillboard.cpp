#include "qeheader.h"


void QeBillboard::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	mtlData = AST->getMaterialImage(AST->getXMLValue(editProperty, 1, "image"));
	bufferData.material = mtlData->value;
	AST->setGraphicsShader(graphicsShader, editProperty, "billboard");
}

QeDataDescriptorSetModel QeBillboard::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageView = mtlData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSampler = mtlData->image.pBaseColorMap->sampler;
	return descriptorSetData;
}

void QeBillboard::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, 1, 1, 0, 0);
}