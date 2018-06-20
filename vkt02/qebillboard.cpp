#include "qeheader.h"


void QeBillboard::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	mtlData = AST->getMaterialImage(AST->getXMLValue(editProperty, 1, "image"));

	AST->setGraphicsShader(mtlData->shader, editProperty, "billboard");
}

QeDataDescriptorSetModel QeBillboard::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageViews = mtlData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = mtlData->image.pBaseColorMap->sampler;
	return descriptorSetData;
}

void QeBillboard::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&mtlData->shader, eGraphicsPipeLine_Point, bAlpha);
}

void QeBillboard::setMatModel() {
	
	//type = eBillboardFaceAndSize;
	QeMatrix4x4f mat;
	mat *= MATH->translate(pos);
	mat *= MATH->scale(size);

	bufferData.model = mat;
	bufferData.param.x = float(type);

	if (parentOID) {
		QePoint* p = OBJMGR->getObject(parentOID);
		if (p)	bufferData.model = p->getAttachMatrix(attachSkeletonName.c_str())*bufferData.model;

		if (bFollowColor) {
			if (p->objectType == eObject_Light) {
				QeLight * light = (QeLight*)p;
				mtlData->value.baseColor = light->bufferData.color;
			}
		}  
	}
}

void QeBillboard::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, 1, 1, 0, 0);
}