#include "qeheader.h"


void QeRender::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);
	//modelData = AST->getModel("plane");

	shaderKey = "render";
	AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

	AST->getXMLiValue((int*)&renderSize.width, initProperty, 1, "width");
	AST->getXMLiValue((int*)&renderSize.height, initProperty, 1, "height");
	AST->getXMLiValue(&targetCameraOID, initProperty, 1, "targetCameraOID");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
	graphicsPipeline.bAlpha = false;
	GRAP->models.push_back(this);

	GRAP->getRender(eRender_color, targetCameraOID, renderSize);
	VK->createDescriptorSet(descriptorSet);
	//VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
	bUpdate = false;
}

QeDataDescriptorSetModel QeRender::createDescriptorSetModel() {

	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	QeDataRender* render = GRAP->getRender(eRender_color, targetCameraOID, renderSize);

	if (render->subpass.size()>0)
		descriptorSet.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	else
		descriptorSet.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	descriptorSetData.baseColorMapImageView = render->colorImage.view;
	descriptorSetData.baseColorMapSampler = render->colorImage.sampler;
	return descriptorSetData;
}


void QeRender::update1() {

	if (!bUpdate) {
		VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
		bUpdate = true;
	}

	QeVector3f scale = owner->transform->worldScale();
	scale.x *= MATH->fastSqrt((float(renderSize.width) / renderSize.height));
	bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(), scale);

	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}