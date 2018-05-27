#include "qeheader.h"

QeModel::~QeModel() {
	cleanupPipeline();
}

void QeModel::cleanupPipeline() {

	if (graphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, graphicsPipeline, nullptr);
		graphicsPipeline = VK_NULL_HANDLE;
	}
	if (computePipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, computePipeline, nullptr);
		computePipeline = VK_NULL_HANDLE;
	}
	if (normalPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, normalPipeline, nullptr);
		normalPipeline = VK_NULL_HANDLE;
	}
}

void QeModel::createPipeline() {
	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, ePipeLine_Triangle, bAlpha);

	if (VK->bShowNormal && normalShader.vert) {
		normalPipeline = VK->createGraphicsPipeline(&normalShader, ePipeLine_Line);
	}
}

void QeModel::setPosFaceUpSize(QeVector3f& _pos, float _face, float _up, QeVector3f& _size) {
	pos = _pos;
	face = _face;
	up = _up;
	size = _size;
}

void QeModel::move(QeVector3f& _pos) {
	pos += _pos;
}
void QeModel::setPosition(QeVector3f& _pos) {
	pos = _pos;
}

void QeModel::rotateFace(float _face) {
	face += _face;

	while (face > 360) face -= 360;
	while (face < -360) face += 360;
}

void QeModel::setFace(float _face) {
	face = _face;

	while (face > 360) face -= 360;
	while (face < -360) face += 360;
}

void QeModel::rotateUp(float _up) {
	up += _up;

	while (up > 360) up -= 360;
	while (up < -360) up += 360;
}

void QeModel::setUp(float _up) {
	up = _up;

	while (up > 360) up -= 360;
	while (up < -360) up += 360;
}

void QeModel::enlarge(QeVector3f& _size) {
	size += _size;
}
void QeModel::setSize(QeVector3f& _size) {
	size = _size;
}

void QeModel::setMatModel() {

	QeMatrix4x4f mat;

	mat *= MATH->translate(pos);
	mat *= MATH->rotateY(up);
	//mat *= MATH->rotate(up, QeVector3f(0.0f, 1.0f, 0.0f));
	mat *= MATH->rotateZ(face);
	//mat *= MATH->rotate(face, QeVector3f(0.0f, 0.0f, 1.0f));
	mat *= MATH->scale(size);
	
	ubo.model = mat;

	if (attachID > 0) {
		QeModel* model = OBJMGR->getModel(attachID,nullptr);
		if (model != nullptr) {
			ubo.model = model->getAttachMatrix(attachSkeletonName)*ubo.model;
		}
	}
}

QeMatrix4x4f QeModel::getAttachMatrix(const char* attachSkeletonName) {

	if (attachSkeletonName == nullptr)	return ubo.model;

	size_t size = modelData->jointsAnimation.size();
	if( size == 0 ) return ubo.model;

	size_t i;
	for (i = 0; i<size ;++i ) {
		if ( strcmp(attachSkeletonName, modelData->jointsAnimation[i].name) == 0) break;
	}

	if (i == size) return ubo.model;

	return ubo.model*joints[i];
}


void QeModel::init(QeAssetXML* _property) {

	initProperty = _property;

	const char* c = AST->getXMLValue(_property, 1, "obj");
	modelData = AST->getModel(c);
	pMaterial = modelData->pMaterial;
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;

	data.uboBuffer = uboBuffer.buffer;
	//QeLight* light = OBJMGR->getLight(0, nullptr);
	data.lightBuffer = ACT->light->uboBuffer.buffer;
	data.materialBuffer = pMaterial->uboBuffer.buffer;
	data.diffuseMapImageViews = pMaterial->image.pDiffuseMap->view;
	data.diffueMapSamplers = pMaterial->image.pDiffuseMap->sampler;
	data.normalMapImageViews = pMaterial->image.pNormalMap->view;
	data.normalMapSamplers = pMaterial->image.pNormalMap->sampler;

	cubeMapID = 0;
	c = AST->getXMLValue(_property, 1, "cubemapid");
	if (c != nullptr)	cubeMapID = atoi(c);

	ubo.param.z = 0;

	if (cubeMapID > 0) {
		QeModel* model = OBJMGR->getCube(cubeMapID, nullptr);
		if (model != nullptr) {
			data.cubeMapImageViews = model->pMaterial->image.pCubeMap->view;
			data.cubeMapSamplers = model->pMaterial->image.pCubeMap->sampler;
			ubo.param.z = 1;
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
	attachID=0;
	attachSkeletonName=nullptr;
	actionSpeed = 0;

	if (modelData->animationNum > 0)
		AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "Action"));
	else if (pMaterial->type == eMaterialPBR)
		AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "model"));
	else 
		AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "obj"));

	AST->setShader(normalShader, nullptr, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "normal") );

	c = AST->getXMLValue(_property, 1, "alpha");
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
	else if (modelData->rootJoint) {
		actionPlay();
		updateAction(0);
		actionStop();
	}

	c = AST->getXMLValue(_property, 1, "attachid");
	if (c != nullptr)	attachID = atoi(c);

	attachSkeletonName = AST->getXMLValue(_property, 1, "attachskeleton");
}

void QeModel::updateRender(float time) {

	updateShowByCulling();
	if(speed != 0)	rotateFace( time*speed );
	updateAction(time);
	updateUniformBuffer();
}

void QeModel::updateShowByCulling() {
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

	if (_bCullingShow) {
		float angle = MATH->getAnglefromVectors(camera->face, pos - camera->pos);
		if (angle > 100 || angle < -100) _bCullingShow = false;
	}

	if (_bCullingShow != bCullingShow) {
		bCullingShow = _bCullingShow;
		VP->bUpdateDrawCommandBuffers = true;
	}
}

void QeModel::setShow(bool b) {
	if (b != bShow) {
		bShow = b;
		VP->bUpdateDrawCommandBuffers = true;
	}
}

void QeModel::updateCompute(float time) {}

void QeModel::updateUniformBuffer() {
	/*int size1 = sizeof(float);
	int size2 = sizeof(int);
	int size3 = sizeof(QeVector3f);
	int size4 = sizeof(QeVector4f);
	int size5 = sizeof(QeVector3i);
	int size6 = sizeof(QeMatrix4x4f);
	int size7 = sizeof(QeMatrix4x4f)*9;
	int size8 = sizeof(QeUniformBufferObject);
	int size9 = sizeof(ubo);*/
	//pos = VP->getTargetCamera()->target;
	setMatModel();

	ubo.ambientColor = ACT->ambientColor;
	ubo.param.x = float(VP->currentNum);
	for (int i = 0; i < VP->currentNum; ++i) {
		ubo.view[i] = VP->cameras[i]->view;
		ubo.proj[i] = VP->cameras[i]->projection;

		QeMatrix4x4f mat = ubo.view[i]* ubo.model;
		MATH->inverse(mat, mat);
		ubo.normal[i] = MATH->transpose(mat);
		ubo.cameraPos[i] = VP->cameras[i]->pos;
		ubo.cameraPos[i].w = 1;
	}
	VK->setMemory(uboBuffer.memory,(void*)&ubo, sizeof(ubo));
	
	//QeLight* light = OBJMGR->getLight(0,nullptr);
	//VK->setMemory(light->uboBuffer.memory, (void*)(&(light->data)), sizeof(light->data));

	//if (modelData->pMaterial->type == eMaterial)	
	//	VK->setMemory(modelData->pMaterial->uboBuffer.memory, (void*)&modelData->pMaterial->value, sizeof(modelData->pMaterial->value));
	//else if (modelData->pMaterial->type == eMaterialPBR)
	//	VK->setMemory(modelData->pMaterial->uboBuffer.memory, (void*)&modelData->pMaterial->valuePBR, sizeof(modelData->pMaterial->valuePBR));
}

bool QeModel::setAction(unsigned int actionID, QeActionType type) {
	if (actionID <= modelData->animationNum) {
		currentActionID = actionID;
		actionType = type;
		return true;
	}
	return false;
}

void QeModel::actionPlay()	{

	currentActionFrame = modelData->animationStartFrames[currentActionID];
	currentActionTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
	actionState = eActionStatePlay;
}

void QeModel::actionPause() {	actionState = eActionStatePause; }
void QeModel::actionStop()	{	actionState = eActionStateStop; }

void QeModel::updateAction(float time) {

	if (actionState != eActionStatePlay) return;

	float previousActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame];
	float nextActionFrameTime = modelData->jointsAnimation[0].rotationInput[currentActionFrame + 1];
	bool bFinalFrame = false;
	if ((currentActionFrame + 1) == modelData->animationEndFrames[currentActionID]) bFinalFrame = true;

	float progessive = MATH->clamp((currentActionTime- previousActionFrameTime)/(nextActionFrameTime-previousActionFrameTime),0.f,1.f);

	QeVector3f previousTranslation, nextTranslation, currentTranslation, currentScale;
	QeVector4f previousRotation, nextRotation, currentRotation;
	currentScale = {1,1,1};

	size_t size = modelData->jointsAnimation.size();

	for (size_t i = 0;i<size;++i ) {
		previousTranslation = modelData->jointsAnimation[i].translationOutput[currentActionFrame];
		nextTranslation = modelData->jointsAnimation[i].translationOutput[currentActionFrame + 1];
		currentTranslation = MATH->interpolatePos(previousTranslation, nextTranslation, progessive);

		previousRotation = modelData->jointsAnimation[i].rotationOutput[currentActionFrame];
		nextRotation = modelData->jointsAnimation[i].rotationOutput[currentActionFrame+1];
		currentRotation = MATH->interpolateDir(previousRotation, nextRotation, progessive);
		joints[i] = MATH->transform(currentTranslation, currentRotation, currentScale);
	}

	QeMatrix4x4f mat;
	setChildrenJointTransform(*modelData->rootJoint, mat);

	currentActionTime += time*actionSpeed;
	if (currentActionTime > nextActionFrameTime) {

		if (bFinalFrame) {
			if (actionType == eActionTypeOnce) actionStop();
			else {
				if (actionType == eActionTypeNext) {
					++currentActionID;
					if (currentActionID >= modelData->animationNum) currentActionID -= modelData->animationNum;
				}
				actionPlay();
			}
		}
		else	++currentActionFrame;
	}
}

void QeModel::setChildrenJointTransform(QeDataJoint& joint, QeMatrix4x4f &parentTransform) {

	size_t size = modelData->jointsAnimation.size();
	QeVector3f scale(1,1,1);

	for (size_t i = 0; i<size ;++i ) {
		if (modelData->jointsAnimation[i].id == joint.id) {

			joints[i] = parentTransform*joints[i];

			size_t size1 = modelData->jointsAnimation[i].children.size();
			for (size_t j = 0; j<size1 ;++j)
				setChildrenJointTransform( *modelData->jointsAnimation[i].children[j], joints[i]);
			
			ubo.joints[i] = joints[i]*modelData->jointsAnimation[i].inverseBindMatrix;
			break;
		}
	}
}

void QeModel::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;

	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
	vkCmdBindIndexBuffer(drawCommandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdDrawIndexed(drawCommandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
	//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

	if (VK->bShowNormal && normalPipeline ) {
		vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		vkCmdBindIndexBuffer(drawCommandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, normalPipeline);
		vkCmdDrawIndexed(drawCommandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
	}
}