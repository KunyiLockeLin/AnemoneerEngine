#pragma once

#include "qeheader.h"


struct QeVertex {
	QeVector3f pos;
	QeVector3f color;
	QeVector2f texCoord;
	QeVector3f normal;
	QeVector4f tangent;
	QeVector4f joint;
	QeVector4f weight;

	QeVertex() {}
	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions();
	bool operator==(const QeVertex& other) const; 
};

struct QeDataJoint {

	unsigned char id = 0;
	std::vector<QeDataJoint*> children;
	const char* name = nullptr;
	//QeVector3f translation;
	//QeVector4f rotation;
	//QeVector3f scale;
	//QeMatrix4x4f transform;
	QeMatrix4x4f inverseBindMatrix;
	std::vector<float> translationInput;
	std::vector<QeVector3f> translationOutput;
	std::vector<float> rotationInput;
	std::vector<QeVector4f> rotationOutput;
	//std::vector<float> scaleInput;
	//std::vector<QeVector3f> scaleOutput;
};

struct QeAssetModel {

	std::vector<QeVertex> vertices;
	std::vector<uint32_t> indices;
	size_t indexSize = 0;

	QeVKBuffer vertex;
	QeVKBuffer index;

	QeAssetMaterial* pMaterial = nullptr;
	QeDataJoint* rootJoint = nullptr;
	std::vector<QeDataJoint> jointsAnimation;
	unsigned char animationNum = 0;
	std::vector<unsigned int> animationStartFrames;
	std::vector<unsigned int> animationEndFrames;

	~QeAssetModel();
};

enum QeMaterialType {
	eMaterialPhong,
	eMaterialPBR,
};

struct QeDataMaterialPhong {
	QeVector4f diffuse;
	QeVector4f ambient;
	QeVector4f specular;
	QeVector4f emissive;
	QeVector4f param; // 1: specularExponent(1f), 2: refraction(1f), 3: alpha(1f),
};

struct QeDataMaterialPBR {
	QeVector4f baseColor;
	QeVector4f metallicRoughness;
	QeVector4f emissive;
};

struct QeDataMaterial {
	union{
		QeDataMaterialPhong phong;
		QeDataMaterialPBR pbr;
	};
};

struct QeAssetImage {

	QeVKImageBuffer buffer;
	VkSampler sampler=VK_NULL_HANDLE;

	~QeAssetImage();
};

struct QeAssetMaterial {

	QeMaterialType type = eMaterialPhong;
	
	union QeDataMaterial {
		QeDataMaterialPhong phong;
		QeDataMaterialPBR pbr;
		QeDataMaterial() {}
	}value;

	QeVKBuffer	uboBuffer;
	QeAssetImage* pDiffuseMap; // baseColorMap
	QeAssetShader* pShaderVert;
	QeAssetShader* pShaderGeom;
	QeAssetShader* pShaderFrag;

	~QeAssetMaterial();
};

struct QeAssetShader {

	VkShaderModule shader = VK_NULL_HANDLE;
	~QeAssetShader();
};

enum QeAssetType {
	eAssetModel,
	eAssetMaterial,
	eAssetBin,
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

	~QeAssetXML();
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

	~QeAssetJSON();
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
	~QeAsset();

	QeAssetJSON* getJSON(const char* _filePath);
	const char*	 getJSONValue(int length, ...);
	const char*	 getJSONValue(QeAssetJSON* source, int length, ...);
	const char*	 getJSONValue(QeAssetJSON* source, const char* keys[], int length);
	QeAssetJSON* getJSONNode(int length, ...);
	QeAssetJSON* getJSONNode(QeAssetJSON* source, int length, ...);
	QeAssetJSON* getJSONNode(QeAssetJSON* source, const char* keys[], int length);
	std::vector<std::string>*	getJSONArrayValues(int length, ...);
	std::vector<std::string>*	getJSONArrayValues(QeAssetJSON* source, int length, ...);
	std::vector<std::string>*	getJSONArrayValues(QeAssetJSON* source, const char* keys[], int length);
	std::vector<QeAssetJSON*>*	getJSONArrayNodes(int length, ...);
	std::vector<QeAssetJSON*>*	getJSONArrayNodes(QeAssetJSON* source, int length, ...);
	std::vector<QeAssetJSON*>*	getJSONArrayNodes(QeAssetJSON* source, const char* keys[], int length);

	QeAssetXML* getXML(const char* _filePath);
	const char* getXMLValue(int length, ...);
	const char* getXMLValue(QeAssetXML* source, int length, ...);
	const char* getXMLValue(QeAssetXML* source, const char* keys[], int length);
	QeAssetXML* getXMLNode(int length, ...);
	QeAssetXML* getXMLNode(QeAssetXML* source, int length, ...);
	QeAssetXML* getXMLNode(QeAssetXML* source, const char* keys[], int length);

	QeAssetModel* getModel(const char* _filename);
	QeAssetMaterial* getMaterial(const char* _filename);
	QeAssetImage* getImage(const char* _filename);
	QeAssetShader* getShader(const char* _filename);

	std::vector<char> loadFile(const char* _filePath);
	void imageFillto32bits(std::vector<unsigned char>* data, int bytes);
	std::string combinePath(const char* _filename, QeAssetType dataType);
};