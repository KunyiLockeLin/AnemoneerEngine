#include "qeheader.h"

QeModel::~QeModel() {
	cleanupPipeline();
}

void QeModel::cleanupPipeline() {
	graphicsPipeline = VK_NULL_HANDLE;
	normalPipeline = VK_NULL_HANDLE;

	/*if (graphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, graphicsPipeline, nullptr);
		graphicsPipeline = VK_NULL_HANDLE;
	}
	if (normalPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, normalPipeline, nullptr);
		normalPipeline = VK_NULL_HANDLE;
	}*/
	if (computePipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(VK->device, computePipeline, nullptr);
		computePipeline = VK_NULL_HANDLE;
	}
}

void QeModel::updateShaderData() {
	size_t size = VP->viewports.size();
	size_t i = shaderData.size();

	if ( size>i ) {
		//shaderData.resize(size);
		for (; i < size; ++i) {
			QeDataModelShader * data = new QeDataModelShader();
			shaderData.push_back(data);

			//VK->createBuffer(data->buffer, sizeof(data->data), nullptr);
			VK->createDescriptorSet(shaderData[i]->descriptorSet);
			VK->updateDescriptorSet(&createDescriptorSetModel(int(i)), shaderData[i]->descriptorSet);
		}
	}
	for (; i > size; --i) {
		delete shaderData[i - 1];
		shaderData.pop_back();
	}
}

QeDataDescriptorSetModel QeModel::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageViews = pMaterial->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = pMaterial->image.pBaseColorMap->sampler;
	descriptorSetData.normalMapImageViews = pMaterial->image.pNormalMap->view;
	descriptorSetData.normalMapSamplers = pMaterial->image.pNormalMap->sampler;
	//descriptorSetData.modelViewportBuffer = shaderData[index]->buffer.buffer;

	bufferData.param.x = 0;
	if (cubeMapID > 0) {
		QeModel* model = OBJMGR->getCube(cubeMapID, nullptr);
		if (model != nullptr) {
			descriptorSetData.cubeMapImageViews = model->pMaterial->image.pCubeMap->view;
			descriptorSetData.cubeMapSamplers = model->pMaterial->image.pCubeMap->sampler;
			bufferData.param.x = 1;
		}
	}
	return descriptorSetData;
}

void QeModel::createPipeline() {

	graphicsPipeline = VK->createGraphicsPipeline(&pMaterial->shader, eGraphicsPipeLine_Triangle, bAlpha);

	if (VK->bShowNormal && normalShader.vert) {
		normalPipeline = VK->createGraphicsPipeline(&normalShader, eGraphicsPipeLine_Point);
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
	
	bufferData.model = mat;

	if (attachID > 0) {
		QeBase* base = OBJMGR->getPoint(attachID, nullptr);
		if (base) {
			bufferData.model._30 += base->pos.x;
			bufferData.model._31 += base->pos.y;
			bufferData.model._32 += base->pos.z;
		}
		else {
			QeModel* model = OBJMGR->getModel(attachID, nullptr);
			if (model != nullptr) {
				bufferData.model = model->getAttachMatrix(attachSkeletonName)*bufferData.model;
			}
		}
	}
}

QeMatrix4x4f QeModel::getAttachMatrix(const char* attachSkeletonName) {

	if (attachSkeletonName == nullptr)	return bufferData.model;

	size_t size = modelData->jointsAnimation.size();
	if( size == 0 ) return bufferData.model;

	size_t i;
	for (i = 0; i<size ;++i ) {
		if ( strcmp(attachSkeletonName, modelData->jointsAnimation[i].name) == 0) break;
	}

	if (i == size) return bufferData.model;

	return bufferData.model*joints[i];
}


void QeModel::init(QeAssetXML* _property) {

	initProperty = _property;

	const char* c = AST->getXMLValue(_property, 1, "obj");
	modelData = AST->getModel(c);
	pMaterial = modelData->pMaterial;

	if (modelData->animationNum > 0)
		AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "Action"));
	else //if (pMaterial->type == eMaterialPBR)
		AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "model"));
	//else
		//AST->setShader(pMaterial->shader, initProperty, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "obj"));

	AST->setShader(normalShader, nullptr, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "normal"));

	setProperty();
}

void QeModel::setProperty() {

	pos = { 0, 0, 0 };
	face = 0.0f;
	up = 0.0f;

	if(modelData)	size = modelData->scale;
	else			size = { 1,1,1 };

	speed = 0;
	currentActionID = 0;
	actionType = eActionTypeOnce;
	actionState = eActionStateStop;
	attachID = 0;
	attachSkeletonName = nullptr;
	actionSpeed = 0;
	cubeMapID = 0;

	AST->getXMLiValue(cubeMapID, *initProperty, 1, "cubemapid");
	AST->getXMLbValue(bAlpha, *initProperty, 1, "alpha");
	AST->getXMLuiValue(id, *initProperty, 1, "id");
	AST->getXMLfValue(pos.x, *initProperty, 1, "posX");
	AST->getXMLfValue(pos.y, *initProperty, 1, "posY");
	AST->getXMLfValue(pos.z, *initProperty, 1, "posZ");
	AST->getXMLfValue(size.x, *initProperty, 1, "scaleX");
	AST->getXMLfValue(size.y, *initProperty, 1, "scaleY");
	AST->getXMLfValue(size.z, *initProperty, 1, "scaleZ");
	AST->getXMLiValue(speed, *initProperty, 1, "speed");
	AST->getXMLbValue(bShow, *initProperty, 1, "show");
	AST->getXMLiValue(cullingDistance, *initProperty, 1, "culling");

	if (modelData && modelData->rootJoint) {
		AST->getXMLfValue(actionSpeed, *initProperty, 1, "actionSpeed");
		AST->getXMLuiValue(currentActionID, *initProperty, 1, "action");
		int i = 0;
		AST->getXMLiValue(i, *initProperty, 1, "actionType");

		actionType = QeActionType(i);
		setAction(currentActionID, actionType);
		actionPlay();
		updateAction(0);

		if (!AST->getXMLValue(initProperty, 1, "action")) {
			actionStop();
		}
	}

	AST->getXMLiValue(attachID, *initProperty, 1, "attachid");

	attachSkeletonName = AST->getXMLValue(initProperty, 1, "attachskeleton");

	AST->getXMLiValue(particleID, *initProperty, 1, "paritcleid");

	bufferData.material = pMaterial->value;

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
}

void QeModel::updateCompute(float time) {
	updateShowByCulling();
	if (speed != 0)	rotateFace(time*speed);
	updateAction(time);
	updateBuffer();
}
void QeModel::updateRender(float time) {
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

void QeModel::updateBuffer() {

	setMatModel();
	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
	
	/*size_t size = shaderData.size();
	size_t size1 = VP->viewports.size();
	if (size1 < size) size = size1;

	for (size_t i = 0; i < size; ++i) {
		QeCamera* camera = VP->viewports[i]->camera;
		QeMatrix4x4f mat = camera->bufferData.view* bufferData.model;
		MATH->inverse(mat, mat);
		shaderData[i]->data.normal = MATH->transpose(mat);
		VK->setMemoryBuffer(shaderData[i]->buffer, sizeof(shaderData[i]->data), &shaderData[i]->data);
	}*/
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

	if (!modelData || actionState != eActionStatePlay) return;

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
			
			bufferData.joints[i] = joints[i]*modelData->jointsAnimation[i].inverseBindMatrix;
			break;
		}
	}
}

std::vector<VkDescriptorSet> QeModel::getDescriptorSets(int index) {
	std::vector<VkDescriptorSet> descriptorSets = 
	{	
		shaderData[index]->descriptorSet.set,
		VP->viewports[index]->commonDescriptorSet.set
	};
	return descriptorSets;
}

void QeModel::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	if (!bShow || !bCullingShow) return;
	
	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
	vkCmdBindIndexBuffer(drawCommandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDrawIndexed(drawCommandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
	//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

	if (VK->bShowNormal && normalPipeline->graphicsPipeline) {
		vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		//vkCmdBindIndexBuffer(drawCommandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, normalPipeline->graphicsPipeline);
		//vkCmdDrawIndexed(drawCommandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
		vkCmdDraw(drawCommandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
	}
}