#include "qeasset.h"


VkVertexInputBindingDescription QeVertex::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(QeVertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> QeVertex::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(QeVertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(QeVertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(QeVertex, texCoord);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(QeVertex, normal);

	return attributeDescriptions;
}

bool QeVertex::operator==(const QeVertex& other) const {
	return pos == other.pos && normal == other.normal && texCoord == other.texCoord && color == other.color;
}


std::vector<char> QeAsset::loadFile(const char* _filename) {
	std::ifstream file(_filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

QeAssetModel* QeAsset::loadModelOBJ(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetModel);
	std::map<std::string, void*>::iterator it = asset.find(_filePath);

	if (it != asset.end())	return static_cast<QeAssetModel*>(it->second);

	std::ifstream file(_filePath.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) return nullptr;
	file.seekg(0);

	char line[500];
	std::vector<QeVertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<QeVector3f> normalV;
	std::vector<QeVector2f> texCoordV;

	QeVector3f tempV3;
	QeVector2f tempV2;
	QeVector3i tempV3p, tempV3t, tempV3n;
	char mtlPath[200];

	while (file.getline(line, sizeof(line))) {
		
		if (line[0] == 'm' && line[1] == 't' && line[2] == 'l' && line[3] == 'l' && line[4] == 'i' && line[5] == 'b') {
			sscanf_s(line, "mtllib  %s", mtlPath,  (unsigned int)sizeof(mtlPath) );
		}
		else if (line[0] == 'v' && line[1] == ' ') {
			sscanf_s(line, "v %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
			QeVertex vet;
			vet.pos = tempV3;
			vet.color ={ 1.0f, 1.0f, 1.0f };
			vertices.push_back(vet);
		} 
		else if (line[0] == 'v' && line[1] == 't') {
			sscanf_s(line, "vt %f %f", &(tempV2.x), &(tempV2.y));
			texCoordV.push_back(tempV2);
		}
		else if (line[0] == 'v' && line[1] == 'n') {
			sscanf_s(line, "vn %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
			normalV.push_back(tempV3);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			
			if (strstr(line, "//")) {
				sscanf_s(line, "f %d//%d %d//%d %d//%d", &(tempV3p.x), &(tempV3n.x),
					&(tempV3p.y), &(tempV3n.y), &(tempV3p.z), &(tempV3n.z));
			}
			else if (texCoordV.empty()) {
				sscanf_s(line, "f %d %d %d", &(tempV3p.x), &(tempV3p.y), &(tempV3p.z));
			}
			else if (normalV.empty()) {
				sscanf_s(line, "f %d/%d %d/%d %d/%d", &(tempV3p.x), &(tempV3t.x),
					&(tempV3p.y), &(tempV3t.y), &(tempV3p.z), &(tempV3t.z));
			}
			else{
				sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &(tempV3p.x), &(tempV3t.x),
					&(tempV3n.x), &(tempV3p.y), &(tempV3t.y), &(tempV3n.y), &(tempV3p.z), &(tempV3t.z), &(tempV3n.z));
			}
			
			tempV3p -= 1;

			if (!texCoordV.empty()) {
				tempV3t -= 1;
				vertices[tempV3p.x].texCoord = texCoordV[tempV3t.x];
				vertices[tempV3p.y].texCoord = texCoordV[tempV3t.y];
				vertices[tempV3p.z].texCoord = texCoordV[tempV3t.z];
			}
			if (!normalV.empty()) {
				tempV3n -= 1;
				vertices[tempV3p.x].normal = normalV[tempV3n.x];
				vertices[tempV3p.y].normal = normalV[tempV3n.y];
				vertices[tempV3p.z].normal = normalV[tempV3n.z];
			}
			indices.push_back(tempV3p.x);
			indices.push_back(tempV3p.y);
			indices.push_back(tempV3p.z);
		}
	}
	file.close();

	QeAssetModel* model = new QeAssetModel();

	createVertexBuffer(*model, vertices);
	createIndexBuffer(*model, indices);
	model->indexSize = indices.size();

	asset[_filePath] = model;

	if (strlen(mtlPath) != 0)
		model->pMaterial = loadMateialMTL(mtlPath);

	return model;
}

QeAssetMaterial* QeAsset::loadMateialMTL(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetMaterial);
	std::map<std::string, void*>::iterator it = asset.find(_filePath.c_str());

	if (it != asset.end())	return static_cast<QeAssetMaterial*>(it->second);

	std::ifstream file(_filePath.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) return nullptr;
	file.seekg(0);

	QeAssetMaterial* mtl = new QeAssetMaterial();
	char line[500];
	char diffuseMapPath[500];

	while (file.getline(line, sizeof(line))) {

		if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p' && line[3] == '_' && line[4] == 'K' && line[5] == 'd') 
			sscanf_s(line, "map_Kd %s", diffuseMapPath, (unsigned int) sizeof(diffuseMapPath));
	
		else if (line[0] == 'N' && line[1] == 's') 
			sscanf_s(line, "Ns %f", &mtl->specularExponent);
		
		else if (line[0] == 'K' && line[1] == 'a') 
			sscanf_s(line, "Ka %f %f %f", &(mtl->ambient.x), &(mtl->ambient.y), &(mtl->ambient.z));

		else if (line[0] == 'K' && line[1] == 'd') 
			sscanf_s(line, "Kd %f %f %f", &(mtl->diffuse.x), &(mtl->diffuse.y), &(mtl->diffuse.z));

		else if (line[0] == 'K' && line[1] == 's') 
			sscanf_s(line, "Ks %f %f %f", &(mtl->specular.x), &(mtl->specular.y), &(mtl->specular.z));
	
		else if (line[0] == 'K' && line[1] == 'e') 
			sscanf_s(line, "Ke %f %f %f", &(mtl->emissive.x), &(mtl->emissive.y), &(mtl->emissive.z));

		else if (line[0] == 'N' && line[1] == 'i') 
			sscanf_s(line, "Ni %f", &(mtl->refraction));
	
		else if (line[0] == 'd' && line[1] == ' ') 
			sscanf_s(line, "d %f", &(mtl->alpha));
	}

	file.close();

	asset[_filePath] = mtl;

	if (strlen(diffuseMapPath) != 0)
		mtl->pDiffuseMap = loadImageBMP32(diffuseMapPath);

	return mtl;
}

QeAssetImage* QeAsset::loadImageBMP32(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetTexture);
	std::map<std::string, void*>::iterator it = asset.find(_filePath.c_str());
	if (it != asset.end())	return static_cast<QeAssetImage*>(it->second);

	std::ifstream _file(_filePath.c_str(), std::ios::ate | std::ios::binary);
	if (!_file.is_open()) return nullptr;
	_file.seekg(0);

	char header[122];
	_file.read(header, sizeof(header));

	if (header[0] != 'B' || header[1] != 'M') {
		_file.close();
		return false;
	}
	
	int dataPos = *(int*)&(header[0x0A]);
	int width = *(int*)&(header[0x12]);
	int height = *(int*)&(header[0x16]);
	int imageSize = *(int*)&(header[0x22]);

	if (imageSize == 0)
		imageSize = width *height * 4;
	if (dataPos == 0)
		dataPos = sizeof(header);
	
	std::vector<char> data;
	data.resize(imageSize);
	
	_file.seekg(dataPos);
	_file.read(data.data(), imageSize);
	_file.close();

	QeAssetImage* image = new QeAssetImage();
	createTextureImage(*image, data, width, height, imageSize);
	createTextureImageView(*image);
	createTextureSampler(*image);
	asset[_filePath] = image;
	return image;
}


void QeAsset::createTextureImage(QeAssetImage& image, std::vector<char>& imageData, int width, int height, int imageSize) {

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	QE->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(QE->device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, imageData.data(), static_cast<size_t>(imageSize));
	vkUnmapMemory(QE->device, stagingBufferMemory);

	QE->createImage(width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image.textureImage, image.textureImageMemory);

	QE->transitionImageLayout(image.textureImage, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	QE->copyBufferToImage(stagingBuffer, image.textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	QE->transitionImageLayout(image.textureImage, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(QE->device, stagingBuffer, nullptr);
	vkFreeMemory(QE->device, stagingBufferMemory, nullptr);
}

void QeAsset::createTextureImageView(QeAssetImage& image) {
	image.textureImageView = QE->createImageView(image.textureImage, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void QeAsset::createTextureSampler(QeAssetImage& image) {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(QE->device, &samplerInfo, nullptr, &image.textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void QeAsset::createVertexBuffer(QeAssetModel& model, std::vector<QeVertex>& vertices) {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	QE->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(QE->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(QE->device, stagingBufferMemory);

	QE->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.vertexBuffer, model.vertexBufferMemory);

	QE->copyBuffer(stagingBuffer, model.vertexBuffer, bufferSize);

	vkDestroyBuffer(QE->device, stagingBuffer, nullptr);
	vkFreeMemory(QE->device, stagingBufferMemory, nullptr);
}

void QeAsset::createIndexBuffer(QeAssetModel& model, std::vector<uint32_t>& indices) {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	QE->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(QE->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(QE->device, stagingBufferMemory);

	QE->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model.indexBuffer, model.indexBufferMemory);

	QE->copyBuffer(stagingBuffer, model.indexBuffer, bufferSize);

	vkDestroyBuffer(QE->device, stagingBuffer, nullptr);
	vkFreeMemory(QE->device, stagingBufferMemory, nullptr);
}

bool QeAsset::loadConfig() {

	std::ifstream file(CONFIG_PATH.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) return false;
	file.seekg(0);

	char line[500];

	while (file.getline(line, sizeof(line))) {
		char node[100];
		char value[400];
		sscanf_s(line, "%s %s", node, (unsigned int)sizeof(node), value, (unsigned int)sizeof(value));
		assetString[node] = value;
	}
	file.close();
	return true;
}

const char* QeAsset::getString(const char* _nodeName) {
	std::map<std::string, std::string>::iterator it = assetString.find(_nodeName);
	if (it != assetString.end())	return it->second.c_str();

	return nullptr;
}

std::vector<char>* QeAsset::loadShader(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetShader);
	std::vector<char> data = loadFile(_filePath.c_str());
	std::vector<char>* pData = new std::vector<char>();
	pData->assign(data.begin(), data.end());
	asset[_filename] = pData;
	return pData;
}

std::string QeAsset::combinePath(const char* _filename, QeAssetType dataType) {

	if (strlen(_filename) > 3 && _filename[1] == ':' && _filename[2] == '\\')	return _filename;

	std::string rtn;
	switch (dataType) {
	
	case eAssetModel:
		rtn = getString( "MODEL_PATH" );
		break;
	case eAssetMaterial:
		rtn = getString("MTL_PATH");
		break;
	case eAssetShader:
		rtn = getString("SHADER_PATH");
		break;
	case eAssetTexture:
		rtn = getString("TEXTURE_PATH");
		break;
	}
	return rtn.append(_filename).c_str();
}