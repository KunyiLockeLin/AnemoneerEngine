#include "qeheader.h"


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

std::vector<char> QeAsset::loadFile(const char* _filePath) {

	std::vector<char> ret;
	std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) return ret;

	file.seekg(0, file.end);
	int length = int(file.tellg());
	file.seekg(0);

	ret.resize(length);
	file.read(ret.data(), length);
	file.close();
	return ret;
}

QeAssetJSON* QeAsset::getJSON(const char* _filePath) {
	std::map<std::string, QeAssetJSON*>::iterator it = astJSONs.find(_filePath);

	if (it != astJSONs.end())	return it->second;

	std::vector<char> buffer = loadFile(_filePath);

	int index = 0;
	QeAssetJSON* head = ENCODE->decodeJSON(buffer.data(), index);
	astJSONs[_filePath] = head;
	return head;
}

const char*	 QeAsset::getJSONValue(int length, ...) {
	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetJSON* source = getJSON(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	const char** keys1 = new const char*[length - 1];
	for (int i = 0; i<(length - 1); ++i)	keys1[i] = va_arg(keys, const char*);

	const char* ret = getJSONValue(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

const char*	 QeAsset::getJSONValue(QeAssetJSON* source, int length, ...) {
	if (source == nullptr)	return nullptr;

	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i< length; ++i)	keys1[i] = va_arg(keys, const char*);

	const char* ret = getJSONValue(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

const char*	 QeAsset::getJSONValue(QeAssetJSON* source, const char* keys[], int length) {
	if (source == nullptr) return nullptr;

	for (int index = 0; index < length; ++index) {

		if (index == (length - 1)) {
			int size = int(source->eKeysforValues.size());
			for (int index1 = 0; index1 < size; ++index1){
				if (strcmp(keys[index], source->eKeysforValues[index1].c_str()) == 0)
					return source->eValues[index1].c_str();
			}
			break;
		}
	
		int size = int(source->eKeysforNodes.size());
		int index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->eKeysforNodes[index1].c_str()) == 0) {
				source = source->eNodes[index1];
				break;
			}
		}
		if (index1 != size) continue;
		
		size = int(source->eKeysforArrayNodes.size());
		index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->eKeysforArrayNodes[index1].c_str()) == 0) {
				int size2 = int(source->eArrayNodes[index1].size());
				int index2 = 0;
				for (; index2<size2;++index2 ) {
					const char* ret = getJSONValue(source->eArrayNodes[index1][index2], &keys[index+1], length - index-1);
					if (ret != nullptr) return ret;
				}
				break;
			}
		}
		break;
	}
	return nullptr;
}

QeAssetJSON* QeAsset::getJSONNode(int length, ...) {
	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetJSON* source = getJSON(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	const char** keys1 = new const char*[length - 1];
	for (int i = 0; i<(length - 1); ++i)	keys1[i] = va_arg(keys, const char*);

	source = getJSONNode(source, keys1, length);
	va_end(keys);
	delete keys1;
	return source;
}

QeAssetJSON* QeAsset::getJSONNode(QeAssetJSON* source, int length, ...) {

	if (source == nullptr)	return nullptr;

	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i< length; ++i)	keys1[i] = va_arg(keys, const char*);

	source = getJSONNode(source, keys1, length);
	va_end(keys);
	delete keys1;
	return source;
}

QeAssetJSON* QeAsset::getJSONNode(QeAssetJSON* source, const char* keys[], int length) {
	if (source == nullptr) return nullptr;

	int size = int(source->eKeysforNodes.size());
	for (int index = 0; index < size; ++index) {
		if (strcmp(keys[0], source->eKeysforNodes[index].c_str()) == 0){
			
			if(length == 1)	return source->eNodes[index];
			
			return getJSONNode(source->eNodes[index], &keys[1], length - 1);
		}
	}

	size = int(source->eKeysforArrayNodes.size());
	for (int index = 0; index<size; ++index) {
		if (strcmp(keys[0], source->eKeysforArrayNodes[index].c_str()) == 0) {

			if ( length == 1 ) return source->eArrayNodes[index][0];

			int size1 = int(source->eArrayNodes[index].size());
		
			for (int index1 = 0; index1<size1; ++index1) {
				QeAssetJSON* ret = getJSONNode(source->eArrayNodes[index][index1], &keys[1], length- 1);
				if (ret != nullptr) return ret;
			}
			break;
		}
	}
	return nullptr;
}

std::vector<std::string>*	QeAsset::getJSONArrayValues(int length, ...) {
	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetJSON* source = getJSON(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	const char** keys1 = new const char*[length - 1];
	for (int i = 0; i<(length - 1); ++i)	keys1[i] = va_arg(keys, const char*);

	std::vector<std::string>* ret = getJSONArrayValues(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

std::vector<std::string>*	QeAsset::getJSONArrayValues(QeAssetJSON* source, int length, ...) {
	if (source == nullptr)	return nullptr;

	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i< length; ++i)	keys1[i] = va_arg(keys, const char*);

	std::vector<std::string>* ret = getJSONArrayValues(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

std::vector<std::string>*	QeAsset::getJSONArrayValues(QeAssetJSON* source, const char* keys[], int length) {
	if (source == nullptr) return nullptr;

	for (int index = 0; index < length; ++index) {

		if (index == (length - 1)) {
			int size = int(source->eKeysforArrayValues.size());
			for (int index1 = 0; index1 < size; ++index1) {
				if (strcmp(keys[index], source->eKeysforArrayValues[index1].c_str()) == 0)
					return &source->eArrayValues[index1];
			}
			break;
		}

		int size = int(source->eKeysforNodes.size());
		int index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->eKeysforNodes[index1].c_str()) == 0) {
				source = source->eNodes[index1];
				break;
			}
		}
		if (index1 != size) continue;

		size = int(source->eKeysforArrayNodes.size());
		index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->eKeysforArrayNodes[index1].c_str()) == 0) {
				int size2 = int(source->eArrayNodes[index1].size());
				int index2 = 0;
				for (; index2<size2; ++index2) {
					std::vector<std::string>* ret = getJSONArrayValues(source->eArrayNodes[index1][index2], &keys[index + 1], length - index - 1);
					if (ret != nullptr) return ret;
				}
				break;
			}
		}
		break;
	}
	return nullptr;
}

std::vector<QeAssetJSON*>*	QeAsset::getJSONArrayNodes(int length, ...) {
	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetJSON* source = getJSON(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	const char** keys1 = new const char*[length - 1];
	for (int i = 0; i<(length - 1); ++i)	keys1[i] = va_arg(keys, const char*);

	std::vector<QeAssetJSON*>* ret = getJSONArrayNodes(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

std::vector<QeAssetJSON*>*	QeAsset::getJSONArrayNodes(QeAssetJSON* source, int length, ...) {
	if (source == nullptr)	return nullptr;

	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i< length; ++i)	keys1[i] = va_arg(keys, const char*);

	std::vector<QeAssetJSON*>* ret = getJSONArrayNodes(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

std::vector<QeAssetJSON*>*	QeAsset::getJSONArrayNodes(QeAssetJSON* source, const char* keys[], int length) {
	if (source == nullptr) return nullptr;

	int size = int(source->eKeysforArrayNodes.size());
	for (int index = 0; index < size; ++index) {
		if (strcmp(keys[0], source->eKeysforArrayNodes[index].c_str()) == 0) {

			if (length == 1) return &source->eArrayNodes[index];

			int size1 = int(source->eArrayNodes[index].size());

			for (int index1 = 0; index1 < size1; ++index1) {
				std::vector<QeAssetJSON*>* ret = getJSONArrayNodes(source->eArrayNodes[index][index1], &keys[1], length - 1);
				if (ret != nullptr) return ret;
			}
			break;
		}
	}

	if (length>1) {
		size = int(source->eKeysforNodes.size());
		for (int index = 0; index < size; ++index) {
			if (strcmp(keys[0], source->eKeysforNodes[index].c_str()) == 0) {

				return getJSONArrayNodes(source->eNodes[index], &keys[1], length - 1);
			}
		}
	}
	return nullptr;
}


QeAssetXML* QeAsset::getXML(const char* _filePath) {
	std::map<std::string, QeAssetXML*>::iterator it = astXMLs.find(_filePath);

	if (it != astXMLs.end())	return it->second;

	std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) return nullptr;

	std::vector<char> buffer = loadFile(_filePath);

	int index = 0;
	QeAssetXML* head = ENCODE->decodeXML(buffer.data(), index);
	astXMLs[_filePath] = head;
	return head;
}

const char* QeAsset::getXMLValue(int length, ...) {

	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetXML* source = getXML(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	const char** keys1 = new const char*[length-1];
	for (int i = 0; i<(length-1); ++i )	keys1[i] = va_arg(keys, const char*);

	const char* ret = getXMLValue(source, keys1, length);
	va_end(keys);
	delete keys1;
	return ret;
}

const char* QeAsset::getXMLValue(QeAssetXML* source, int length, ...) {
	
	if (source == nullptr)	return nullptr;
	
	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i< length; ++i)	keys1[i] = va_arg(keys, const char*);

	const char* ret = getXMLValue(source, keys1, length+1);
	va_end(keys);
	delete keys1;
	return ret;
}

const char* QeAsset::getXMLValue(QeAssetXML* source, const char* keys[], int length) {

	if (source == nullptr) return nullptr;

	for (int index = 0; index < length; ++index) {

		if (index == (length - 1)) {
			if (strcmp(keys[index-1], source->key.c_str()) == 0) return source->value.c_str();
			break;
		}
		else if (index == (length - 2)) {
			int size = int(source->eKeys.size());
			for (int index1 = 0; index1 < size; ++index1)
				if (strcmp(keys[index], source->eKeys[index1].c_str()) == 0) {
					return source->eVaules[index1].c_str();
				}
		}

		int size = int(source->nexts.size());
		int index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->nexts[index1]->key.c_str()) == 0) {
				source = source->nexts[index1];
				break;
			}
		}
		if (index1 == size) break;
	}
	return nullptr;
}

QeAssetXML* QeAsset::getXMLNode(int length, ...) {

	va_list keys;
	va_start(keys, length);

	const char* key = va_arg(keys, const char*);
	QeAssetXML* source = getXML(key);
	if (source == nullptr) {
		va_end(keys);
		return nullptr;
	}
	length--;
	const char** keys1 = new const char*[length];
	for (int i = 0; i<length; ++i )	keys1[i] = va_arg(keys, const char*);

	source = getXMLNode(source, keys1, length);
	va_end(keys);
	delete keys1;
	return source;
}

QeAssetXML* QeAsset::getXMLNode(QeAssetXML* source, int length, ...) {

	if (source == nullptr)	return nullptr;
	
	va_list keys;
	va_start(keys, length);

	const char** keys1 = new const char*[length];
	for (int i = 0; i<length; ++i)	keys1[i] = va_arg(keys, const char*);

	source = getXMLNode(source, keys1, length);
	va_end(keys);
	delete keys1;
	return source;
}

QeAssetXML* QeAsset::getXMLNode(QeAssetXML* source, const char* keys[], int length) {

	if (source == nullptr)	return nullptr;

	for (int index = 0; index < length; ++index) {

		int size = int(source->nexts.size());
		int index1 = 0;
		for (; index1<size; ++index1) {
			if (strcmp(keys[index], source->nexts[index1]->key.c_str()) == 0) {
				source = source->nexts[index1];
				break;
			}
		}
		if (index1 == size) return nullptr;
	}
	return source;
}

QeAssetModel* QeAsset::getModel(const char* _filename) {
	
	char type = 0; // 0:OBJ, 1:GLTF, 2:GLB

	char *ret = strrchr((char*)_filename, '.');

	if (strcmp(ret + 1, "obj") == 0)		type = 0;
	else if (strcmp(ret + 1, "gltf") == 0)	type = 1;
	else if (strcmp(ret + 1, "glb") == 0 ) type = 2;
	else return nullptr;

	std::string _filePath = combinePath(_filename, eAssetModel);
	std::map<std::string, QeAssetModel*>::iterator it = astModels.find(_filePath);

	if (it != astModels.end())	return it->second;

	std::vector<char> buffer = loadFile(_filePath.c_str());
	QeAssetModel* model = nullptr;

	switch (type) {
	case 0:
		model = ENCODE->decodeOBJ(buffer.data());
		break;
	case 1:
		model = ENCODE->decodeGLTF(buffer.data());
		break;
	case 2:
		model = ENCODE->decodeGLB(buffer.data());
		break;
	}

	VLK->createBufferData((void*)model->vertices.data(), sizeof(model->vertices[0]) * model->vertices.size(), model->vertexBuffer, model->vertexBufferMemory);
	VLK->createBufferData((void*)model->indices.data(), sizeof(model->indices[0]) * model->indices.size(), model->indexBuffer, model->indexBufferMemory);
	astModels[_filePath] = model;

	return model;
}

QeAssetMaterial* QeAsset::getMaterial(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetMaterial);
	std::map<std::string, QeAssetMaterial*>::iterator it = astMaterials.find(_filePath.c_str());

	if (it != astMaterials.end())	return it->second;

	std::vector<char> buffer = loadFile(_filePath.c_str());
	QeAssetMaterial* mtl = ENCODE->decodeMTL(buffer.data());;

	VLK->createUniformBuffer(sizeof(QeDataMaterial), mtl->materialBuffer, mtl->materialBufferMemory);
	VLK->setMemory(mtl->materialBufferMemory, (void*)&mtl->value, sizeof(mtl->value));

	astMaterials[_filePath] = mtl;

	return mtl;
}

QeAssetImage* QeAsset::getImage(const char* _filename) {

	char type = 0; // 0:BMP, 1:PNG, 2:JPEG

	char *ret = strrchr((char*)_filename, '.');

	if (strcmp(ret + 1, "bmp") == 0)		type = 0;
	else if (strcmp(ret + 1, "png") == 0)	type = 1;
	else if (strcmp(ret + 1, "jpg") == 0 || strcmp(ret + 1, "jpeg") == 0) type = 2;
	else return nullptr;

	std::string _filePath = combinePath(_filename, eAssetTexture);
	std::map<std::string, QeAssetImage*>::iterator it = astTextures.find(_filePath.c_str());

	if (it != astTextures.end())	return it->second;

	std::vector<char> buffer = loadFile(_filePath.c_str());

	VkFormat format;
	int width, height, bytes;
	std::vector<unsigned char> data; 
	
	switch (type) {
	case 0:
		data = ENCODE->decodeBMP((unsigned char*)buffer.data(), &width, &height, &bytes);
		format = VK_FORMAT_B8G8R8A8_UNORM;
		break;
	case 1:
		data = ENCODE->decodePNG((unsigned char*)buffer.data(), &width, &height, &bytes);
		format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case 2:
		data = ENCODE->decodeJPEG((unsigned char*)buffer.data(), &width, &height, &bytes);
		format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	}
	if (bytes != 4)	imageFillto32bits(&data, bytes);

	QeAssetImage* image = new QeAssetImage();
	VLK->createImageData((void*)data.data(), format, data.size(), width, height, image->textureImage, image->textureImageMemory);
	image->textureImageView = VLK->createImageView(image->textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);

	image->textureSampler = VLK->createTextureSampler();
	astTextures[_filePath] = image;
	return image;
}

void QeAsset::imageFillto32bits(std::vector<unsigned char>* data, int bytes) {
	int fileIndex = 0;
	int dataIndex = 0;
	unsigned char c = 0xff;

	size_t size = data->size();
	size_t index = bytes;
	short int addBytes = 4 - bytes;

	while (index<=size) {
		
		for( int i =0 ; i<addBytes; ++i)	data->insert( data->begin()+index+i, c );

		index += 4;
		size = data->size();
	}
}

QeAssetShader* QeAsset::getShader(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetShader);
	std::map<std::string, QeAssetShader*>::iterator it = astShaders.find(_filePath.c_str());
	if (it != astShaders.end())	return it->second;

	std::vector<char> buffer = loadFile(_filePath.c_str());

	QeAssetShader* shader = new QeAssetShader();
	shader->shader = VLK->createShaderModel((void*)buffer.data(), int(buffer.size()));
	astShaders[_filename] = shader;
	return shader;
}

std::string QeAsset::combinePath(const char* _filename, QeAssetType dataType) {

	if (strlen(_filename) > 3 && _filename[1] == ':' && _filename[2] == '\\')	return _filename;
	
	std::string rtn;
	switch (dataType) {
	
	case eAssetModel:
		rtn = getXMLValue(3, CONFIG, "path", "model");
		break;
	case eAssetMaterial:
		rtn = getXMLValue(3, CONFIG, "path", "material");
		break;
	case eAssetShader:
		rtn = getXMLValue(3, CONFIG, "path", "sharder");
		break;
	case eAssetTexture:
		rtn = getXMLValue(3, CONFIG, "path", "texture");
		break;
	}
	return rtn.append(_filename).c_str();
}