#include "qeheader.h"


void QeCube::init(QeAssetXML* _property) {

	initProperty = _property;

	modelData = AST->getModel("cube", true);
	pMaterial = AST->getMaterialImage(AST->getXMLValue(_property, 1, "image"), true);
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	//VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);
	VK->createBuffer(uboBuffer, sizeof(QeUniformBufferObject), nullptr);
	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	data.cubeMapImageViews = pMaterial->image.pCubeMap->view;
	data.cubeMapSamplers = pMaterial->image.pCubeMap->sampler;
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

	AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "cubemap"));
	createPipeline();

	const char * c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	pos.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	pos.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "culling");
	if (c != nullptr)	cullingDistance = atoi(c);

	c = AST->getXMLValue(_property, 1, "scaleX");
	if (c != nullptr)	size.x *= float(atof(c));
	c = AST->getXMLValue(_property, 1, "scaleY");
	if (c != nullptr)	size.y *= float(atof(c));
	c = AST->getXMLValue(_property, 1, "scaleZ");
	if (c != nullptr)	size.z *= float(atof(c));
}


void QeCube::updateShowByCulling() {
	if (!bShow) return;
	QeCamera* camera = VP->getTargetCamera();
	bool _bCullingShow = true;
	float dis = MATH->distance(pos, camera->pos);

	if (cullingDistance>0) {
		if (dis > cullingDistance) _bCullingShow = false;
	}
	else {
		if (dis > camera->cullingDistance) _bCullingShow = false;
	}

	if (_bCullingShow != bCullingShow) {
		bCullingShow = _bCullingShow;
		VP->bUpdateDrawCommandBuffers = true;
	}
}