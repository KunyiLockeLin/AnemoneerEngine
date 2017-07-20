#include "qemodel.h"


void QeModel::cleanup() {
	//vkDestroySampler(QE->device, textureSampler, nullptr);
	//vkDestroyImageView(QE->device, textureImageView, nullptr);

	//vkDestroyImage(QE->device, textureImage, nullptr);
	//vkFreeMemory(QE->device, textureImageMemory, nullptr);

	vkDestroyBuffer(QE->device, uniformBuffer, nullptr);
	vkFreeMemory(QE->device, uniformBufferMemory, nullptr);

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

	mat = MATH->translate(pos);
	mat *= MATH->rotate(up, QeVector3f(0.0f, 1.0f, 0.0f));
	mat *= MATH->rotate(face, QeVector3f(0.0f, 0.0f, 1.0f));
	mat *= MATH->scale(size);
	
	return mat;
}


void QeModel::init(const char* _filename) {

	modelData = AST->loadModelOBJ(_filename);
	createUniformBuffer();
	updateDescriptorSet();

	pos = QeVector3f(0, 0, 0);
	face = 0.0f;
	up = 0.0f;
	size = QeVector3f(1, 1, 1);
}

void QeModel::createUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	QE->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

void QeModel::updateDescriptorSet() {

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = modelData->pMaterial->pDiffuseMap->textureImageView;
	imageInfo.sampler = modelData->pMaterial->pDiffuseMap->textureSampler;

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = QE->descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = QE->descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(QE->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void QeModel::update() {

	//static auto startTime = std::chrono::high_resolution_clock::now();
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
	//static float timem = 0;
	//timem += 0.01f;
	//setPosition(QeVector3f(timem, 0.0f, 0.0f));
	//setSize(QeVector3f(1.0f + timem, 1.0f + timem, 1.0f + timem ));
	//setFace(timem);
	//setUp(timem);

	updateUniformBuffer();
}

void QeModel::updateUniformBuffer() {

	UniformBufferObject ubo = {};
	ubo.model = getMatModel();
	ubo.view = QE->camera->getMatView();
	ubo.proj = QE->camera->getMatProjection();

	void* data;
	vkMapMemory(QE->device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(QE->device, uniformBufferMemory);
}