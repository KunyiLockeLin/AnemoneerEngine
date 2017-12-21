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
	eAssetXML,
	eAssetJSON,
};

struct QeAssetXML {
	std::string key;
	std::string value;
	std::vector<std::string> eKeys;
	std::vector<std::string> eVaules;
	std::vector<QeAssetXML*> nexts;
};

struct QeAssetJSON {
	std::vector<std::string> eKeysforValues;
	std::vector<std::string> eValues;
	std::vector<std::string> eKeysforNodes;
	std::vector<QeAssetJSON*> eNodes;
	std::vector<std::string> eKeysforArrayValues;
	std::vector<std::vector<std::string>> eArrayValues;
	std::vector<std::string> eKeysforArrayNodes;
	std::vector<std::vector<QeAssetJSON*>> eArrayNodes;
};

class QeAsset
{
public:
	const char* CONFIG = "../data/config.xml";

	std::map<std::string, QeAssetXML*> astXMLs;
	std::map<std::string, QeAssetJSON*> astJSONs;
	std::map<std::string, QeAssetModel*> astModels;
	std::map<std::string, QeAssetMaterial*> astMaterials;
	std::map<std::string, QeAssetShader*> astShaders;
	std::map<std::string, QeAssetImage*> astTextures;

	QeAsset(QeGlobalKey& _key) {}
	~QeAsset() {}

	QeAssetJSON* getJSON(const char* _filePath);
	QeAssetJSON* decodeJSON(const char* buffer, int &index);
	/*const char*	 getJSONValue(int length, ...);
	QeAssetJSON* getJSONNode(int length, ...);
	std::vector<std::string>*	getJSONArrayValues(int length, ...);
	std::vector<QeAssetJSON*>*	getJSONArrayNodes(int length, ...);*/

	QeAssetXML* getXML(const char* _filePath);
	QeAssetXML* decodeXML(const char* buffer, int &index);
	const char* getXMLValue(int length, ...);
	QeAssetXML* getXMLNode(int length, ...);
	const char* getXMLValue(QeAssetXML* source, int length, ...);
	QeAssetXML* getXMLNode(QeAssetXML* source, int length, ...);
	const char* getXMLValue(QeAssetXML* source, const char* keys[], int length);
	QeAssetXML* getXMLNode(QeAssetXML* source, const char* keys[], int length);

	std::string trim(std::string s);
	
	std::string combinePath(const char* _filename, QeAssetType dataType);
	QeAssetModel* getModel(const char* _filename);
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