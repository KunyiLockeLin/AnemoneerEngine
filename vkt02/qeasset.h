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

struct QeAssetMaterial {

	QeVector3f ambient;
	QeVector3f diffuse;
	QeVector3f specular;
	QeVector3f emissive;
	float refraction;
	float specularExponent;
	float alpha;

	QeAssetImage* pDiffuseMap;
	QeAssetShader* pShader;
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

	std::map<std::string, void*> asset;
	std::map<std::string, std::string> assetString;
	QeAsset(QeGlobalKey& _key) {}
	~QeAsset() {}

	std::vector<char>  loadFile(const char* _filename);
	bool loadConfig();
	const char* getString(const char* _nodeName);
	std::string combinePath(const char* _filename, QeAssetType dataType );
	QeAssetModel* loadModelOBJ(const char* _filename);
	QeAssetMaterial* loadMateialMTL(const char* _filename);
	QeAssetImage* loadImageBMP32(const char* _filename);
	QeAssetShader* loadShader(const char* _filename);

	void createVertexBuffer(QeAssetModel& model, std::vector<QeVertex>& vertices);
	void createIndexBuffer(QeAssetModel& model, std::vector<uint32_t>& indices);

	void createTextureImage(QeAssetImage& image, std::vector<char>& data, int width, int height, int imageSize);
	void createTextureImageView(QeAssetImage& image);
	void createTextureSampler(QeAssetImage& image);
	void createShaderModule(QeAssetShader& shader, const std::vector<char>& code);
};

