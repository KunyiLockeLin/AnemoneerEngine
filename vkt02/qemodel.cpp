#include "qeheader.h"

QeModel::~QeModel() {
	cleanupSwapChain();
	vkDestroyDescriptorPool( VK->device, descriptorPool, nullptr);
}

void QeModel::cleanupSwapChain() {
	vkDestroyPipeline(VK->device, graphicsPipeline, nullptr);
}

void QeModel::createSwapChain() {
	graphicsPipeline = VK->createGraphicsPipeline(&modelData->pMaterial->pShaderVert->shader, 
		&modelData->pMaterial->pShaderGeom->shader, &modelData->pMaterial->pShaderFrag->shader);
}

void QeModel::setPosFaceUpSize(QeVector3f _pos, float _face, float _up, QeVector3f _size) {
	pos = _pos;
	face = _face;
	up = _up;
	size = _size;
}

void QeModel::move(QeVector3f _pos) {
	pos += _pos;
}
void QeModel::setPosition(QeVector3f _pos) {
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

void QeModel::enlarge(QeVector3f _size) {
	size += _size;
}
void QeModel::setSize(QeVector3f _size) {
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

	const char* c = AST->getXMLValue(_property, 1, "obj");;
	modelData = AST->getModel(c);
	descriptorPool = VK->createDescriptorPool();
	descriptorSet = VK->createDescriptorSet(descriptorPool);
	//createDescriptorBuffer();
	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	VkBuffer buffers[3];
	int buffersSize[3];
	VkSampler samplersp[1];
	VkImageView imageViews[1];

	buffers[0] = uboBuffer.buffer;
	buffersSize[0] = sizeof(QeUniformBufferObject);

	QeLight* light = OBJMGR->getLight(0, nullptr);

	buffers[1] = light->uboBuffer.buffer;
	buffersSize[1] = sizeof(QeDataLight);
	buffers[2] = modelData->pMaterial->uboBuffer.buffer;
	samplersp[0] = modelData->pMaterial->pDiffuseMap->sampler;
	imageViews[0] = modelData->pMaterial->pDiffuseMap->buffer.view;

	if (modelData->pMaterial->type == eMaterial)		buffersSize[2] = sizeof(QeDataMaterial);
	else if (modelData->pMaterial->type == eMaterialPBR)	buffersSize[2] = sizeof(QeDataMaterialPBR);

	VK->updateDescriptorSet(buffers, buffersSize, VK->modelRender->descriptorSetBufferNumber, 
		samplersp, imageViews, VK->modelRender->descriptorSetTextureNumber, descriptorSet);

	pos = { 0, 0, 0 };
	face = 0.0f;
	up = 0.0f;
	size = { 1, 1, 1 };
	speed = 30;
	currentActionID = 0;
	actionType = eActionTypeOnce;
	actionState = eActionStateStop;
	attachID=0;
	attachSkeletonName=nullptr;
	actionSpeed = 1;

	c = AST->getXMLValue(_property, 1, "shadervert");
	if (c == nullptr) {
		if ( modelData->animationNum==0 )
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "vert");			
		else
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "skeletonvert");
		
		if (c != nullptr)	modelData->pMaterial->pShaderVert = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderVert = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shadergeom");
	if (c == nullptr) {
		if (modelData->pMaterial->type == eMaterial)
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "geom");
		else if (modelData->pMaterial->type == eMaterialPBR) 
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrgeom");
		
		if (c != nullptr)	modelData->pMaterial->pShaderGeom = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderGeom = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shaderfrag");
	if (c == nullptr) {
		if (modelData->pMaterial->type == eMaterial)
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "frag");
		else if (modelData->pMaterial->type == eMaterialPBR) {
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrfrag");
		}
		if (c != nullptr)	modelData->pMaterial->pShaderFrag = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderFrag = AST->getShader(c);
	
	createSwapChain();

	c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	pos.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	pos.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "speed");
	if (c != nullptr)	speed = atoi(c);

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

	c = AST->getXMLValue(_property, 1, "attachid");
	if (c != nullptr)	attachID = atoi(c);

	attachSkeletonName = AST->getXMLValue(_property, 1, "attachskeleton");
}

void QeModel::update(float time) {

	if(speed != 0)	rotateFace( time*speed );
	updateAction(time);
	updateUniformBuffer();
}

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
	setMatModel();

	ubo.ambientColor = QE->currentActivity->ambientColor;
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
		}}
		else	++currentActionFrame;
}}

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