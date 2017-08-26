#include "qemodel.h"


void QeModel::cleanup() {
	//vkDestroySampler(QE->device, textureSampler, nullptr);
	//vkDestroyImageView(QE->device, textureImageView, nullptr);

	//vkDestroyImage(QE->device, textureImage, nullptr);
	//vkFreeMemory(QE->device, textureImageMemory, nullptr);

	//vkDestroyBuffer(QE->device, uniformBuffer, nullptr);
	//vkFreeMemory(QE->device, uniformBufferMemory, nullptr);
	//vkFreeCommandBuffers(QE->device, QE->commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	//vkDestroyBuffer(QE->device, modelData.indexBuffer, nullptr);
	//vkFreeMemory(QE->device, indexBufferMemory, nullptr);

	//vkDestroyBuffer(QE->device, modelData.vertexBuffer, nullptr);
	//vkFreeMemory(QE->device, vertexBufferMemory, nullptr);
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

QeMatrix4x4f QeModel::getMatModel() {

	QeMatrix4x4f mat;

	mat *= MATH->translate(pos);
	mat *= MATH->rotateY(up);
	//mat *= MATH->rotate(up, QeVector3f(0.0f, 1.0f, 0.0f));
	mat *= MATH->rotateZ(face);
	//mat *= MATH->rotate(face, QeVector3f(0.0f, 0.0f, 1.0f));
	mat *= MATH->scale(size);
	
	return mat;
}


void QeModel::init(const char* _filename) {

	modelData = AST->getModelOBJ(_filename);
	descriptorPool = VLK->createDescriptorPool();
	descriptorSet = VLK->createDescriptorSet(descriptorPool);
	createDescriptorBuffer();
	
	VkBuffer buffers[3];
	int buffersSize[3];
	buffers[0] = mvpBuffer;
	buffersSize[0] = sizeof(QeDataMVP);
	buffers[1] = lightBuffer;
	buffersSize[1] = sizeof(QeDataLight);
	buffers[2] = modelData->pMaterial->materialBuffer;
	buffersSize[2] = sizeof(QeDataMaterial);

	VkSampler samplersp[1];
	VkImageView imageViews[1];
	samplersp[0] = modelData->pMaterial->pDiffuseMap->textureSampler;
	imageViews[0] = modelData->pMaterial->pDiffuseMap->textureImageView;

	VLK->updateDescriptorSet(buffers, buffersSize, 3, samplersp, imageViews, 1, descriptorSet);
	graphicsPipeline = VLK->createGraphicsPipeline(modelData->pMaterial->pShaderVert->shader, modelData->pMaterial->pShaderFrag->shader);
	
	pos = QeVector3f(0, 0, 0);
	face = 0.0f;
	up = 0.0f;
	size = QeVector3f(1, 1, 1);
}

void QeModel::createDescriptorBuffer() {
	VkDeviceSize bufferSize = sizeof(QeDataMVP);
	VLK->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mvpBuffer, mvpBufferMemory);
	bufferSize = sizeof(QeDataLight);
	VLK->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, lightBuffer, lightBufferMemory);
}

void QeModel::update(float time) {

	rotateFace( time*50 );
	updateUniformBuffer();
}

void QeModel::updateUniformBuffer() {

	QeDataMVP mvp = {};
	mvp.model = getMatModel();
	mvp.view = QE->activity->camera->getMatView();
	mvp.proj = QE->activity->camera->getMatProjection();

	QeMatrix4x4f mat = mvp.view*mvp.model;
	MATH->inverse(mat, mat);
	mvp.normal = MATH->transpose(mat);

	VLK->setMemory(mvpBufferMemory,(void*)&mvp, sizeof(mvp));
	
	QeDataLight light = OBJMGR->getLight()->data;
	VLK->setMemory(lightBufferMemory, (void*)&light, sizeof(light));
}