#include "qeheader.h"



void QeModel::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeComponent::initialize(_property, _owner);

	AST->getXMLbValue(&graphicsPipeline.bAlpha, initProperty, 1, "alpha");
	AST->getXMLfValue(&bufferData.param.z, initProperty, 1, "lineWidth");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	const char* c = AST->getXMLValue(initProperty, 1, "obj");
	modelData = AST->getModel(c);
	materialData = modelData->pMaterial;
	if(materialData) bufferData.material = materialData->value;

	AST->setGraphicsShader(graphicsShader, initProperty, "model");
	AST->setGraphicsShader(normalShader, nullptr, "normal");
	AST->setGraphicsShader(outlineShader, nullptr, "outline");

	if (graphicsPipeline.bAlpha)	GRAP->alphaModels.push_back(this);
	else							GRAP->models.push_back(this);
}

void QeModel::clear() {
	if (graphicsPipeline.bAlpha)	eraseElementFromVector<QeModel*>(GRAP->alphaModels, this);
	else							eraseElementFromVector<QeModel*>(GRAP->models, this);
}

void QeModel::update1() {

	bool bRotate = true;
	if (componentType == eComponent_billboard || componentType == eComponent_partical) bRotate = false;

	bufferData.model = owner->transform->worldTransformMatrix(bRotate);
	VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}
void QeModel::update2() {}


void QeModel::updateShaderData() {

	if (!descriptorSet.set) {
		VK->createDescriptorSet(descriptorSet);
		VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
	}
}

QeDataDescriptorSetModel QeModel::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;

	if (materialData->image.pBaseColorMap) {
		descriptorSetData.baseColorMapImageView = materialData->image.pBaseColorMap->view;
		descriptorSetData.baseColorMapSampler = materialData->image.pBaseColorMap->sampler;
	}
	if (materialData->image.pNormalMap) {
		descriptorSetData.normalMapImageView = materialData->image.pNormalMap->view;
		descriptorSetData.normalMapSampler = materialData->image.pNormalMap->sampler;
	}
	bufferData.param.x = 0;
	return descriptorSetData;
}

void QeModel::createPipeline() {
	if (computeShader) computePipeline = VK->createComputePipeline(computeShader);
}

bool QeModel::isShowByCulling(QeCamera* camera) {

	bool _bCullingShow = true;
	QeVector3f vec = owner->transform->worldPosition() - camera->owner->transform->worldPosition();
	float dis = MATH->length(vec);

	if (dis > camera->cullingDistance) return false;

	if (componentType != eComponent_cubemap && _bCullingShow) {
		float angle = MATH->getAnglefromVectors(camera->face(), vec);
		if (angle > 100 || angle < -100) return false;
	}

	return true;
}


void QeModel::updateDrawCommandBuffer(QeDataDrawCommand* command) {

	if (!isShowByCulling(command->camera)) return;

	vkCmdBindDescriptorSets(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet.set, 0, nullptr);

	VkDeviceSize offsets[] = { 0 };

	graphicsPipeline.subpass = 0;
	graphicsPipeline.componentType = componentType;
	graphicsPipeline.sampleCount = GRAP->sampleCount;
	graphicsPipeline.renderPass = command->renderPass;
	graphicsPipeline.minorType = eGraphicsPipeLine_none;
	graphicsPipeline.shader = &graphicsShader;

	vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));

	switch (componentType) {
	case eComponent_model:
	case eComponent_cubemap:
	case eComponent_animation:
		vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		vkCmdBindIndexBuffer(command->commandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
		break;

	case eComponent_render:
	case eComponent_billboard:
		vkCmdDraw(command->commandBuffer, 1, 1, 0, 0);
		break;

	case eComponent_line:
	case eComponent_axis:
	case eComponent_grid:
		vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
		break;

		//case eObject_Particle:
		//	break;
	}

	if (bufferData.param.z  && outlineShader.vert) {

		graphicsPipeline.minorType = eGraphicsPipeLine_stencilBuffer;
		graphicsPipeline.shader = &outlineShader;

		vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
		vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
	}

	if (VK->bShowNormal && normalShader.vert) {

		graphicsPipeline.minorType = eGraphicsPipeLine_normal;
		graphicsPipeline.shader = &normalShader;

		vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
		vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
	}
}
/*void QeModel::updateShaderData() {

	if (!descriptorSet.set) {
		VK->createDescriptorSet(descriptorSet);
		VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
	}
	/*size_t size = VP->viewports.size();
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
	for (; i > size; --i) {+		QeModel	{actionState=eActionStateStop (0) actionType=eActionTypeOnce (0) joints=0x000001cdfc453cb0 {{_00=1.00000000 ...}, ...} ...}	QeModel

		delete shaderData[i - 1];
		shaderData.pop_back();
	}
	
}


QeDataDescriptorSetModel QeModel::createDescriptorSetModel() {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;

	if (mtlData->image.pBaseColorMap) {
		descriptorSetData.baseColorMapImageView = mtlData->image.pBaseColorMap->view;
		descriptorSetData.baseColorMapSampler = mtlData->image.pBaseColorMap->sampler;
	}
	if (mtlData->image.pNormalMap) {
		descriptorSetData.normalMapImageView = mtlData->image.pNormalMap->view;
		descriptorSetData.normalMapSampler = mtlData->image.pNormalMap->sampler;
	}
	bufferData.param.x = 0;
	if (cubemapImage) {
		descriptorSetData.cubeMapImageView = cubemapImage->view;
		descriptorSetData.cubeMapSampler = cubemapImage->sampler;
		bufferData.param.x = 1;
	}
	//descriptorSetData.modelViewportBuffer = shaderData[index]->buffer.buffer;

	//bufferData.param.x = 0;
	/*if (cubemapOID > 0) {
		QePoint* p = OBJMGR->getObject(cubemapOID);
		if (p && p->objectType == eObject_Cubemap) {
			QeCubemap * cube = (QeCubemap*)p;
			descriptorSetData.cubeMapImageView = cube->mtlData->image.pCubeMap->view;
			descriptorSetData.cubeMapSampler = cube->mtlData->image.pCubeMap->sampler;
			bufferData.param.x = 1;
		}
	}
	return descriptorSetData;
}

void QeModel::createPipeline() {

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
		dis = MATH->length(GRAP->getTargetCamera()->pos - pos);
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
	if(modelData->pMaterial)	mtlData = modelData->pMaterial;
	else {
		c = AST->getXMLValue(editProperty, 1, "image");
		if(c)	mtlData = AST->getMaterialImage(c);
		else	mtlData = AST->getMaterialImage("");
	}
	AST->getXMLfValue(&mtlData->value.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	bufferData.material = mtlData->value;
	size *= modelData->scale;

	setAction(currentActionID, actionType);
	updateAction(0);

	if (modelData->animationNum)
		AST->setGraphicsShader(graphicsShader, editProperty, "action");
	else
		AST->setGraphicsShader(graphicsShader, editProperty, "model");
	
	AST->setGraphicsShader(normalShader, nullptr, "normal");
	AST->setGraphicsShader(outlineShader, nullptr, "outline");

}

void QeModel::setProperty() {
	QePoint::setProperty();

	graphicsPipeline.subpass = 0;
	graphicsPipeline.objectType = objectType;
	graphicsPipeline.sampleCount = GRAP->sampleCount;

	face = 0.0f;
	up = 0.0f;

	bFixSize = false;
	AST->getXMLbValue(&bFixSize, initProperty, 1, "fixSize");

	//cubemapOID = 0;
	//AST->getXMLiValue(&cubemapOID, initProperty, 1, "cubemapOID");
	cubemapImage = nullptr;
	const char *c = AST->getXMLValue(editProperty, 1, "cubemapImage");
	if (c) cubemapImage = AST->getImage(c, true);


	cameraOID = 0;
	AST->getXMLiValue(&cameraOID, initProperty, 1, "cameraOID");

	graphicsPipeline.bAlpha = false;
	AST->getXMLbValue(&graphicsPipeline.bAlpha, editProperty, 1, "alpha");

	size = { 1.0f ,1.0f ,1.0f };
	AST->getXMLfValue(&size.x, initProperty, 1, "scaleX");
	AST->getXMLfValue(&size.y, initProperty, 1, "scaleY");
	AST->getXMLfValue(&size.z, initProperty, 1, "scaleZ");

	speed = 0;
	AST->getXMLiValue(&speed, initProperty, 1, "speed");

	bShow = true;
	AST->getXMLbValue(&bShow, initProperty, 1, "show");

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

	renderSize.width = 0;
	AST->getXMLiValue((int*)&renderSize.width, initProperty, 1, "width");

	renderSize.height = 0;
	AST->getXMLiValue((int*)&renderSize.height, initProperty, 1, "height");

	VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

	bufferData.param.z = 0.0f;
	AST->getXMLfValue(&bufferData.param.z, initProperty, 1, "lineWidth");
	//graphicsPipeline.bStencilBuffer = bufferData.param.z ? true : false;
}

void QeModel::update1() {
	if (speed)	rotateFace(QE->deltaTime*speed);
	updateAction(QE->deltaTime);
	updateBuffer();
}

bool QeModel::isShowByCulling(QeCamera* camera) {
	if (!bShow) return false;
	bool _bCullingShow = true;
	float dis = MATH->length(pos - camera->pos);

	if (cullingDistance) {
		if (dis > cullingDistance) return false;
	}
	else {
		if (dis > camera->cullingDistance) return false;
	}

	if (objectType != eObject_Cubemap && _bCullingShow) {
		float angle = MATH->getAnglefromVectors(camera->face, pos - camera->pos);
		if (angle > 100 || angle < -100) return false;
	}

	return true;
}

void QeModel::setShow(bool b) {
	if (b != bShow) {
		bShow = b;
		GRAP->bUpdateDrawCommandBuffers = true;
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
	}
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

void QeModel::updateDrawCommandBuffer(QeDataDrawCommand* command) {

	if (!bShow || !isShowByCulling(command->camera)) return;
	//if (command->type != eRender_main && objectType == eObject_Render) return;

	//STACK("AAAAAAAAAAAAAAAAA");
	vkCmdBindDescriptorSets(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet.set, 0, nullptr);

	VkDeviceSize offsets[] = { 0 };

	graphicsPipeline.renderPass = command->renderPass;
	graphicsPipeline.minorType = eGraphicsPipeLine_none;
	graphicsPipeline.shader = &graphicsShader;
	
	vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));

	switch (objectType) {
	case eObject_Model:
	case eObject_Cubemap:
		vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		vkCmdBindIndexBuffer(command->commandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
		break;

	case eObject_Render:
	case eObject_Billboard:
		vkCmdDraw(command->commandBuffer, 1, 1, 0, 0);
		break;

	case eObject_Line:
		vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
		vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
		break;

	//case eObject_Particle:
	//	break;
	}

	if (bufferData.param.z  && outlineShader.vert) {
		
		graphicsPipeline.minorType = eGraphicsPipeLine_stencilBuffer;
		graphicsPipeline.shader = &outlineShader;

		vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
		vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
	}

	if (VK->bShowNormal && normalShader.vert ) {

		graphicsPipeline.minorType = eGraphicsPipeLine_normal;
		graphicsPipeline.shader = &normalShader;

		vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
		vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
	}
}*/