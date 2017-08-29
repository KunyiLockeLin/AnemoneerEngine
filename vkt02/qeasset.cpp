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

QeAssetModel* QeAsset::getModelOBJ(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetModel);
	std::map<std::string, QeAssetModel*>::iterator it = astModels.find(_filePath);

	if (it != astModels.end())	return it->second;

	std::ifstream file(_filePath.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) return nullptr;
	file.seekg(0);

	char line[500];

	std::vector<QeVector3f> normalV;
	std::vector<QeVector2f> texCoordV;

	QeVector3f tempV3;
	QeVector2f tempV2;
	QeVector3i tempV3p, tempV3t, tempV3n;
	char mtlPath[200];
	
	QeAssetModel* model = new QeAssetModel();

	while (file.getline(line, sizeof(line))) {
		
		if (line[0] == 'm' && line[1] == 't' && line[2] == 'l' && line[3] == 'l' && line[4] == 'i' && line[5] == 'b') {
			sscanf_s(line, "mtllib  %s", mtlPath,  (unsigned int)sizeof(mtlPath) );
		}
		else if (line[0] == 'v' && line[1] == ' ') {
			sscanf_s(line, "v %f %f %f", &(tempV3.x), &(tempV3.y), &(tempV3.z));
			QeVertex vet;
			vet.pos = tempV3;
			vet.color ={ 1.0f, 1.0f, 1.0f };
			model->vertices.push_back(vet);
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
				model->vertices[tempV3p.x].texCoord = texCoordV[tempV3t.x];
				model->vertices[tempV3p.y].texCoord = texCoordV[tempV3t.y];
				model->vertices[tempV3p.z].texCoord = texCoordV[tempV3t.z];
			}
			if (!normalV.empty()) {
				tempV3n -= 1;
				model->vertices[tempV3p.x].normal = normalV[tempV3n.x];
				model->vertices[tempV3p.y].normal = normalV[tempV3n.y];
				model->vertices[tempV3p.z].normal = normalV[tempV3n.z];
			}
			model->indices.push_back(tempV3p.x);
			model->indices.push_back(tempV3p.y);
			model->indices.push_back(tempV3p.z);
		}
	}
	file.close();

	VLK->createBufferData((void*)model->vertices.data(), sizeof(model->vertices[0]) * model->vertices.size(), model->vertexBuffer, model->vertexBufferMemory);
	VLK->createBufferData((void*)model->indices.data(), sizeof(model->indices[0]) * model->indices.size(), model->indexBuffer, model->indexBufferMemory);

	model->indexSize = model->indices.size();

	astModels[_filePath] = model;

	if (strlen(mtlPath) != 0)
		model->pMaterial = getMateialMTL(mtlPath);

	return model;
}

QeAssetMaterial* QeAsset::getMateialMTL(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetMaterial);
	std::map<std::string, QeAssetMaterial*>::iterator it = astMaterials.find(_filePath.c_str());

	if (it != astMaterials.end())	return it->second;

	std::ifstream file(_filePath.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) return nullptr;
	file.seekg(0);

	QeAssetMaterial* mtl = new QeAssetMaterial();
	char line[500];
	char diffuseMapPath[500];
	QeDataMaterial mtl1;
	mtl1.ambient.w = 1;
	mtl1.diffuse.w = 1;
	mtl1.specular.w = 1;
	mtl1.emissive.w = 1;
	char sv[500]="";
	char sg[500]="";
	char sf[500]="";

	while (file.getline(line, sizeof(line))) {

		if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p' && line[3] == '_' && line[4] == 'K' && line[5] == 'd') 
			sscanf_s(line, "map_Kd %s", diffuseMapPath, (unsigned int) sizeof(diffuseMapPath));
	
		else if (line[0] == 'N' && line[1] == 's') 
			sscanf_s(line, "Ns %f", &mtl1.specularExponent);
		
		else if (line[0] == 'K' && line[1] == 'a') 
			sscanf_s(line, "Ka %f %f %f", &(mtl1.ambient.x), &(mtl1.ambient.y), &(mtl1.ambient.z));

		else if (line[0] == 'K' && line[1] == 'd') 
			sscanf_s(line, "Kd %f %f %f", &(mtl1.diffuse.x), &(mtl1.diffuse.y), &(mtl1.diffuse.z));

		else if (line[0] == 'K' && line[1] == 's') 
			sscanf_s(line, "Ks %f %f %f", &(mtl1.specular.x), &(mtl1.specular.y), &(mtl1.specular.z));
	
		else if (line[0] == 'K' && line[1] == 'e') 
			sscanf_s(line, "Ke %f %f %f", &(mtl1.emissive.x), &(mtl1.emissive.y), &(mtl1.emissive.z));

		else if (line[0] == 'N' && line[1] == 'i') 
			sscanf_s(line, "Ni %f", &(mtl1.refraction));
	
		else if (line[0] == 'd' && line[1] == ' ') 
			sscanf_s(line, "d %f", &(mtl1.alpha));

		else if (line[0] == 's' && line[1] == 'v')
			sscanf_s(line, "sv %s", sv, (unsigned int) sizeof(sv));
		else if (line[0] == 's' && line[1] == 'g')
			sscanf_s(line, "sg %s", sg, (unsigned int) sizeof(sg));
		else if (line[0] == 's' && line[1] == 'f')
			sscanf_s(line, "sf %s", sf, (unsigned int) sizeof(sf));
		 
	}

	file.close();
	mtl->value = mtl1;
	VLK->createUniformBuffer(sizeof(QeDataMaterial), mtl->materialBuffer, mtl->materialBufferMemory);
	VLK->setMemory(mtl->materialBufferMemory, (void*)&mtl1, sizeof(mtl1));

	astMaterials[_filePath] = mtl;

	if (strlen(diffuseMapPath) != 0)
		mtl->pDiffuseMap = getImageBMP32(diffuseMapPath);

	if(strlen(sv) == 0)
		mtl->pShaderVert = getShader(getString("defaultshadervert"));
	else
		mtl->pShaderVert = getShader(sv);

	if (strlen(sg) == 0)
		mtl->pShaderGeom = getShader(getString("defaultshadergeom"));
	else
		mtl->pShaderGeom = getShader(sg);

	if (strlen(sf) == 0)
		mtl->pShaderFrag = getShader(getString("defaultshaderfrag"));
	else
		mtl->pShaderFrag = getShader(sf);

	return mtl;
}

QeAssetImage* QeAsset::getImageBMP32(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetTexture);
	std::map<std::string, QeAssetImage*>::iterator it = astTextures.find(_filePath.c_str());
	if (it != astTextures.end())	return it->second;

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
	VLK->createImageData((void*)data.data(), imageSize, width, height, image->textureImage, image->textureImageMemory);
	image->textureImageView = VLK->createImageView(image->textureImage, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	image->textureSampler = VLK->createTextureSampler();
	astTextures[_filePath] = image;
	return image;
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

		std::map<std::string, std::vector<std::string>>::iterator it = astString.find(node);
		
		if (it != astString.end() && !it->second.empty()) {
			it->second.push_back( value );
		}
		else {
			std::vector<std::string> s;
			s.push_back( value );
			astString[node] = s;
		}
	}
	file.close();
	return true;
}

const char* QeAsset::getString(const char* _nodeName, int _index ) {
	std::map<std::string, std::vector<std::string>>::iterator it = astString.find(_nodeName);
	if (it != astString.end() && !it->second.empty() ) 
		return it->second.at(_index).c_str();

	return nullptr;
}

QeAssetShader* QeAsset::getShader(const char* _filename) {

	std::string _filePath = combinePath(_filename, eAssetShader);
	std::map<std::string, QeAssetShader*>::iterator it = astShaders.find(_filePath.c_str());
	if (it != astShaders.end())	return it->second;

	std::vector<char> data = loadFile(_filePath.c_str());

	QeAssetShader* shader = new QeAssetShader();
	shader->shader = VLK->createShaderModel((void*)data.data(), int(data.size()));
	astShaders[_filename] = shader;
	return shader;
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