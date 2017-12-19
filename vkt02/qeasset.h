#pragma once

#include "qeheader.h"


struct QeVertex {
	QeVector3f pos;
	QeVector3f color;
	QeVector2f texCoord;
	QeVector3f normal;

	QeVertex() {}
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
	bool operator==(const QeVertex& other) const; 
};

struct QeAssetModel {

	std::vector<QeVertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;
	size_t indexSize;

	QeAssetMaterial* pMaterial;
};

struct QeDataMaterial {
	QeVector4f ambient;
	QeVector4f diffuse;
	QeVector4f specular;
	QeVector4f emissive;
	QeVector4f param; // 1: specularExponent, 2: refraction, 3: alpha,
};

struct QeAssetMaterial {

	QeDataMaterial value;
	VkBuffer materialBuffer;
	VkDeviceMemory materialBufferMemory;
	QeAssetImage* pDiffuseMap;
	QeAssetShader* pShaderVert;
	QeAssetShader* pShaderGeom;
	QeAssetShader* pShaderFrag;
};

struct QeAssetImage {

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

struct QeAssetShader {

	VkShaderModule shader;
};


enum QeAssetType {
	eAssetModel,
	eAssetMaterial,
	eAssetShader,
	eAssetTexture,
};

class QeAsset
{
public:
	const std::string CONFIG_PATH = "../data/config.ini";

	std::map<std::string, QeAssetModel*> astModels;
	std::map<std::string, QeAssetMaterial*> astMaterials;
	std::map<std::string, QeAssetShader*> astShaders;
	std::map<std::string, QeAssetImage*> astTextures;
	std::map<std::string, std::vector<std::string>> astString;
	QeAsset(QeGlobalKey& _key) {}
	~QeAsset() {}

	std::vector<char>  loadFile(const char* _filename);
	bool loadConfig();
	const char* getString(const char* _nodeName, int _index = 0);
	void readJSON(const char* _nodeName);
	void readXML(const char* _nodeName);
	std::string combinePath(const char* _filename, QeAssetType dataType);
	//QeAssetModel* getModel(const char* _filename);
	QeAssetModel* getModelOBJ(const char* _filename);
	QeAssetModel* getModelGLTF(const char* _filename);
	//QeAssetModel* getModelGLB(const char* _filename);
	QeAssetMaterial* getMateialMTL(const char* _filename);
	//QeAssetImage* getImage(const char* _filename);
	QeAssetImage* getImageBMP32(const char* _filename);
	//QeAssetImage* getImageJPG(const char* _filename);
	//QeAssetImage* getImagePNG(const char* _filename);
	QeAssetShader* getShader(const char* _filename);

};