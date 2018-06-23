#include "qeheader.h"


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
	descriptorSetData.baseColorMapImageViews = mtlData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = mtlData->image.pBaseColorMap->sampler;
	descriptorSetData.normalMapImageViews = mtlData->image.pNormalMap->view;
	descriptorSetData.normalMapSamplers = mtlData->image.pNormalMap->sampler;
	//descriptorSetData.modelViewportBuffer = shaderData[index]->buffer.buffer;

	bufferData.param.x = 0;
	if (cubemapOID > 0) {
		QePoint* p = OBJMGR->getObject(cubemapOID);
		if (p && p->objectType == eObject_Cubemap) {
			QeCubemap * cube = (QeCubemap*)p;
			descriptorSetData.cubeMapImageViews = cube->mtlData->image.pCubeMap->view;
			descriptorSetData.cubeMapSamplers = cube->mtlData->image.pCubeMap->sampler;
			bufferData.param.x = 1;
		}
	}
	return descriptorSetData;
}

void QeModel::createPipeline() {

	QeGraphicsPipelineType type;

	switch (objectType) {
	case eObject_Model:
	case eObject_Cubemap:
		type = eGraphicsPipeLine_Triangle;
		break;
	case eObject_Billboard:
	case eObject_Line:
	case eObject_Particle:
		type = eGraphicsPipeLine_Point;
		break;
	}

	graphicsPipeline = VK->createGraphicsPipeline(&graphicsShader, type, bAlpha);

	if (VK->bShowNormal && normalShader.vert) {
		normalPipeline = VK->createGraphicsPipeline(&normalShader, eGraphicsPipeLine_Point);
	}
	if(computeShader) computePipeline = VK->createComputePipeline(computeShader);
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

	bool bRotate = true;

	if (objectType == eObject_Billboard || objectType == eObject_Particle) bRotate = false;

	QeMatrix4x4f mat;
	mat *= MATH->translate(pos);

	if (bRotate) {
		mat *= MATH->rotateY(up);
		//mat *= MATH->rotate(up, QeVector3f(0.0f, 1.0f, 0.0f));
		mat *= MATH->rotateZ(face);
		//mat *= MATH->rotate(face, QeVector3f(0.0f, 0.0f, 1.0f));
	}
	float dis = 1.0f;
	if (bFixSize) {
		dis = MATH->length(VP->getTargetCamera()->pos - pos);
		dis = dis < 0.1f ? 0.01f : dis/10;
	}
	mat *= MATH->scale(size*dis);
	
	bufferData.model = mat;

	if (parentOID) {
		QePoint* p = OBJMGR->getObject(parentOID);
		if (p) {
			if (bFollowColor) {
				if (p->objectType == eObject_Model) {
					QeModel * pm = (QeModel*)p;
					bufferData.material.baseColor = pm->mtlData->value.baseColor;
				}
				else if (p->objectType == eObject_Light) {
					QeLight * pl = (QeLight*)p;
					bufferData.material.baseColor = pl->bufferData.color;
				}
			}
			parentModel = p->getAttachMatrix(attachSkeletonName.c_str());
			if (bFollowPos) {
				if (!bRotate) {
					parentModel._00 = 1; parentModel._01 = 0; parentModel._02 = 0;
					parentModel._10 = 0; parentModel._11 = 1; parentModel._12 = 0;
					parentModel._20 = 0; parentModel._21 = 0; parentModel._22 = 1;
				}
				bufferData.model = parentModel *bufferData.model;
			}
		}
	}
}

QeMatrix4x4f QeModel::getAttachMatrix(const char* attachSkeletonName) {

	if (!attachSkeletonName || !strlen(attachSkeletonName))	return bufferData.model;

	size_t size = modelData->jointsAnimation.size();
	if( !size ) return bufferData.model;

	for (size_t i = 0; i<size ;++i ) {
		if (!strcmp(attachSkeletonName, modelData->jointsAnimation[i].name))
				return bufferData.model*joints[i];
	}
	return bufferData.model;
}

void QeModel::init(QeAssetXML* _property, int _parentOID) {

	QePoint::init(_property, _parentOID);
	
	const char* c = AST->getXMLValue(editProperty, 1, "obj");
	modelData = AST->getModel(c);
	mtlData = modelData->pMaterial;
	bufferData.material = mtlData->value;
	size *= modelData->scale;

	setAction(currentActionID, actionType);
	updateAction(0);

	if (modelData->animationNum)
		AST->setGraphicsShader(graphicsShader, editProperty, "action");
	else
		AST->setGraphicsShader(graphicsShader, editProperty, "model");
	
	AST->setGraphicsShader(normalShader, nullptr, "normal");
}

void QeModel::setProperty() {
	QePoint::setProperty();

	face = 0.0f;
	up = 0.0f;

	bFixSize = false;
	AST->getXMLbValue(&bFixSize, initProperty, 1, "fixSize");

	cubemapOID = 0;
	AST->getXMLiValue(&cubemapOID, initProperty, 1, "cubemapOID");
	
	bAlpha = false;
	AST->getXMLbValue(&bAlpha, editProperty, 1, "alpha");

	size = { 1.0f ,1.0f ,1.0f };
	AST->getXMLfValue(&size.x, initProperty, 1, "scaleX");
	AST->getXMLfValue(&size.y, initProperty, 1, "scaleY");
	AST->getXMLfValue(&size.z, initProperty, 1, "scaleZ");

	speed = 0;
	AST->getXMLiValue(&speed, initProperty, 1, "speed");

	bShow = true;
	AST->getXMLbValue(&bShow, initProperty, 1, "show");

	bCullingShow = false;
	cullingDistance = 0;
	AST->getXMLiValue(&cullingDistance, initProperty, 1, "culling");

	currentActionFrame = 0;
	currentActionTime = 0.0f;

	actionSpeed = 0;
	AST->getXMLfValue(&actionSpeed, initProperty, 1, "actionSpeed");
	
	actionState = eActionStateStop;
	AST->getXMLiValue((int*)&actionState, initProperty, 1, "actionState");
	
	actionType = eActionTypeOnce;
	AST->getXMLiValue((int*)&actionType, initProperty, 1, "actionType");
	
	currentActionID = 0;
	AST->getXMLiValue(&currentActionID, initProperty, 1, "action");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
}

void QeModel::updateCompute(float time) {
	updateShowByCulling();
	if (speed)	rotateFace(time*speed);
	updateAction(time);
	updateBuffer();
}

void QeModel::updateShowByCulling() {
	if (!bShow) return;
	QeCamera* camera = VP->getTargetCamera();
	bool _bCullingShow = true;
	float dis = MATH->length(pos - camera->pos);

	if (cullingDistance) {
		if (dis > cullingDistance) _bCullingShow = false;
	}
	else {
		if (dis > camera->cullingDistance) _bCullingShow = false;
	}

	if (objectType != eObject_Cubemap && _bCullingShow) {
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

	if (!modelData || !modelData->rootJoint || (actionState != eActionStatePlay && time )) return;

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
	std::vector<VkDescriptorSet> descriptorSets = {	
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
		vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, normalPipeline->graphicsPipeline);
		vkCmdDraw(drawCommandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
	}
}