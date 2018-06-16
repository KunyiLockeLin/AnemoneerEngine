#include "qeheader.h"


void QeBillboard::init(QeAssetXML* _property) {

	initProperty = _property;

	//modelData = AST->getModel("point");
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"));

	AST->setShader(pMaterial->shader, _property, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "billboard"));
	
	setProperty();
}

QeDataDescriptorSetModel QeBillboard::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = shaderData[index]->modelBuffer.buffer;
	descriptorSetData.baseColorMapImageViews = pMaterial->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = pMaterial->image.pBaseColorMap->sampler;
	return descriptorSetData;
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

	bufferData.model = mat;
	bufferData.param.x = float(type);
}

//void QeBillboard::updateRender(float time) { updateUniformBuffer(); }
//void QeBillboard::updateCompute(float time) {}

void QeBillboard::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;

	std::array<VkDescriptorSet, 2> descriptorSets1 = { VP->viewports[VP->currentCommandViewport]->commonDescriptorSet.descriptorSet, shaderData[VP->currentCommandViewport]->descriptorSet.descriptorSet };
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets1.size()), descriptorSets1.data(), 0, nullptr);

	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, 1, 1, 0, 0);
}