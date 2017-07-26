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

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;
	size_t indexSize;

	QeAssetMaterial* pMaterial;
};

struct QeDataMaterial {
	QeVector3f ambient;
	QeVector3f diffuse;
	QeVector3f specular;
	QeVector3f emissive;
	float refraction;
	float specularExponent;
	float alpha;
};

struct QeAssetMaterial {

	//QeDataMaterial* pMaterial;
	VkBuffer materialBuffer;
	VkDeviceMemory materialBufferMemory;
	QeAssetImage* pDiffuseMap;
	QeAssetShader* pShaderVert;
	QeAssetShader* pShaderFarg;
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
	const char* getString(const char* _nodeName, int _index=0);
	std::string combinePath(const char* _filename, QeAssetType dataType );
	QeAssetModel* getModelOBJ(const char* _filename);
	QeAssetMaterial* getMateialMTL(const char* _filename);
	QeAssetImage* getImageBMP32(const char* _filename);
	QeAssetShader* getShader(const char* _filename);

	void createVertexBuffer(QeAssetModel& model, std::vector<QeVertex>& vertices);
	void createIndexBuffer(QeAssetModel& model, std::vector<uint32_t>& indices);

	void createTextureImage(QeAssetImage& image, std::vector<char>& data, int width, int height, int imageSize);
	void createTextureImageView(QeAssetImage& image);
	void createTextureSampler(QeAssetImage& image);
	void createShaderModule(QeAssetShader& shader, const std::vector<char>& code);
};
