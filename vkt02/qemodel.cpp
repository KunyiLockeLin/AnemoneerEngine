#include "qeheader.h"

QeModel::~QeModel() {

	VLK->destroyBufferMemory(uboBuffer, uboBufferMemory);
	VLK->destroyBufferMemory(lightBuffer, lightBufferMemory);
	cleanupSwapChain();
	vkDestroyDescriptorPool( VLK->device, descriptorPool, nullptr);
}

void QeModel::cleanupSwapChain() {
	vkDestroyPipeline(VLK->device, graphicsPipeline, nullptr);
}

void QeModel::createSwapChain() {
	graphicsPipeline = VLK->createGraphicsPipeline(&modelData->pMaterial->pShaderVert->shader, &modelData->pMaterial->pShaderGeom->shader, &modelData->pMaterial->pShaderFrag->shader);
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
}


void QeModel::init(const char* _filename) {

	modelData = AST->getModel(_filename);
	descriptorPool = VLK->createDescriptorPool();
	descriptorSet = VLK->createDescriptorSet(descriptorPool);
	createDescriptorBuffer();
	
	VkBuffer buffers[3];
	int buffersSize[3];
	VkSampler samplersp[1];
	VkImageView imageViews[1];

	buffers[0]		= uboBuffer;
	buffersSize[0]	= sizeof(QeUniformBufferObject);
	buffers[1]		= lightBuffer;
	buffersSize[1]	= sizeof(QeDataLight);
	buffers[2]		= modelData->pMaterial->materialBuffer;
	samplersp[0]	= modelData->pMaterial->pDiffuseMap->textureSampler;
	imageViews[0]	= modelData->pMaterial->pDiffuseMap->textureImageView;

	if (modelData->pMaterial->type == eMaterialNormal)		buffersSize[2]	= sizeof(QeDataMaterial);
	else if (modelData->pMaterial->type == eMaterialPBR)	buffersSize[2]	= sizeof(QeDataPBRMaterial);
	
	VLK->updateDescriptorSet(buffers, buffersSize, VLK->descriptorSetBufferNumber, samplersp, imageViews, VLK->descriptorSetTextureNumber, descriptorSet);

	pos = QeVector3f(0, 0, 0);
	face = 0.0f;
	up = 0.0f;
	size = QeVector3f(1, 1, 1);
}

void QeModel::setProperty(QeAssetXML* _property) {

	const char* c;

	c = AST->getXMLValue(_property, 1, "shadervert");
	if (c == nullptr) {
		if ( modelData->pMaterial->type == eMaterialNormal )
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basevert");			
		else if (modelData->pMaterial->type == eMaterialPBR) {
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrvert");
			if(c == nullptr) c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basevert");
		}
		if (c != nullptr)	modelData->pMaterial->pShaderVert = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderVert = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shadergeom");
	if (c == nullptr) {
		if (modelData->pMaterial->type == eMaterialNormal)
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basegeom");
		else if (modelData->pMaterial->type == eMaterialPBR) {
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrgeom");
			if (c == nullptr) c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basegeom");
		}
		if (c != nullptr)	modelData->pMaterial->pShaderGeom = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderGeom = AST->getShader(c);

	c = AST->getXMLValue(_property, 1, "shaderfrag");
	if (c == nullptr) {
		if (modelData->pMaterial->type == eMaterialNormal)
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basefrag");
		else if (modelData->pMaterial->type == eMaterialPBR) {
			c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "pbrfrag");
			if (c == nullptr) c = AST->getXMLValue(3, AST->CONFIG, "defaultShader", "basefrag");
		}
		if (c != nullptr)	modelData->pMaterial->pShaderFrag = AST->getShader(c);
	}
	else	modelData->pMaterial->pShaderFrag = AST->getShader(c);
	
	createSwapChain();

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	pos.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	pos.z = float(atof(c));
}

void QeModel::createDescriptorBuffer() {
	VLK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer, uboBufferMemory);
	VLK->createUniformBuffer(sizeof(QeDataLight), lightBuffer, lightBufferMemory);
}

void QeModel::update(float time) {

	rotateFace( time*50 );
	updateUniformBuffer();
}

void QeModel::updateUniformBuffer() {
	int size1 = sizeof(float);
	int size2 = sizeof(int);
	int size3 = sizeof(QeVector3f);
	int size4 = sizeof(QeVector4f);
	int size5 = sizeof(QeVector3i);
	int size6 = sizeof(QeMatrix4x4f);
	int size7 = sizeof(QeMatrix4x4f)*9;
	int size8 = sizeof(QeUniformBufferObject);
	int size9 = sizeof(ubo);
	setMatModel();
	ubo.ambientColor = QE->currentActivity->ambientColor;
	ubo.param.x = float(VP->currentNum);
	for (int i = 0; i < VP->currentNum; ++i) {
		ubo.view[i] = VP->cameras[i]->getMatView();
		ubo.proj[i] = VP->cameras[i]->getMatProjection();

		QeMatrix4x4f mat = ubo.view[i]* ubo.model;
		MATH->inverse(mat, mat);
		ubo.normal[i] = MATH->transpose(mat);
		ubo.cameraPos[i] = VP->cameras[i]->pos;
		ubo.cameraPos[i].w = 1;
	}
	VLK->setMemory(uboBufferMemory,(void*)&ubo, sizeof(ubo));
	
	QeDataLight* light = &OBJMGR->getLight(0)->data;
	VLK->setMemory(lightBufferMemory, (void*)light, sizeof(*light));

	if (modelData->pMaterial->type == eMaterialNormal)	
		VLK->setMemory(modelData->pMaterial->materialBufferMemory, (void*)&modelData->pMaterial->value, sizeof(modelData->pMaterial->value));
	else if (modelData->pMaterial->type == eMaterialPBR)
		VLK->setMemory(modelData->pMaterial->materialBufferMemory, (void*)&modelData->pMaterial->pbrValue, sizeof(modelData->pMaterial->pbrValue));
}