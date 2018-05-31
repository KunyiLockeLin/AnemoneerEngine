#pragma once

#include "qeheader.h"


struct QeVertex {
	QeVector4f pos = {0.0f,0.0f,0.0f,1.0f};
	QeVector4f color = { 0.0f,0.0f,0.0f,1.0f };
	QeVector4f texCoord;
	QeVector4f normal = { 0.0f,0.0f,0.0f,1.0f };
	QeVector4f tangent;
	QeVector4f joint;
	QeVector4f weight;

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
	QeVector3f scale;

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
	QeDataMaterial() {}
};

struct QeAssetImage {

	QeVKImage* pDiffuseMap = nullptr; // baseColorMap;
	QeVKImage* pCubeMap = nullptr;;
	QeVKImage* pNormalMap = nullptr;;
};

struct QeAssetShader {

	VkShaderModule vert = VK_NULL_HANDLE;
	VkShaderModule tesc = VK_NULL_HANDLE;
	VkShaderModule tese = VK_NULL_HANDLE;
	VkShaderModule geom = VK_NULL_HANDLE;
	VkShaderModule frag = VK_NULL_HANDLE;
};

struct QeAssetMaterial {

	QeMaterialType type = eMaterialPhong;
	QeVKBuffer	uboBuffer;
	QeAssetImage image;
	QeAssetShader shader;
	QeDataMaterial value;
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

struct QeAssetParticleRule {
	QeVector4f count_life;			// 0: count, 1: count_range, 2: life(second), 3: life_range
	QeVector4f init_pos_lw;			// 0: length, 1: length_range, 2: width, 3: width_range,
	QeVector4f init_pos_hr;			// 0: height, 1: height_range, 2: radius, 3: radius_range,
	QeVector4f init_speed_xy;		// 0: speed_x, 1: speed_x_range, 2: speed_y, 3: speed_y_range
	QeVector4f init_speed_z_force_x;// 0: speed_z, 3: speed_z_range, 2: force_x 1: force_x_range
	QeVector4f force_yz;			// 0: force_y 1: force_y_range, 2:force_z 3: force_z_range
	QeVector4f rotate_xy;			// 0: rotate_x(degree), 1: roate_x_range, 2: rotate_y, 3: rotate_y_range,
	QeVector4f rotate_z_color_r;	// 0: rotate_z, 1: roate_y_range, 2: color_r 3: color_r_range
	QeVector4f color_gb;			// 0: color_g, 1: color_g_range, 2: color_b 3: color_b_range
	QeVector4f alpha_born_size_x;	// 0: bAlpha, 1: bReborn, 2: size_x, 3: size_x_range 
	QeVector4f size_y_init_pos_d;	// 0: size_y, 1: size_y_range, 2: degree, 3: degree_range
	//QeVKImage* image = nullptr;
	const char* image = nullptr;
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
	std::map<std::string, VkShaderModule> astShaders;
	std::map<std::string, QeVKImage*> astTextures;
	std::map<std::string, QeAssetParticleRule*> astParticles;

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
	bool getJSONbValue(bool& output, QeAssetJSON& source, int length, ...);
	bool getJSONiValue(int& output, QeAssetJSON& source, int length, ...);
	bool getJSONfValue(float& output, QeAssetJSON& source, int length, ...);

	QeAssetXML* getXML(const char* _filePath);
	const char* getXMLValue(int length, ...);
	const char* getXMLValue(QeAssetXML* source, int length, ...);
	const char* getXMLValue(QeAssetXML* source, const char* keys[], int length);
	QeAssetXML* getXMLNode(int length, ...);
	QeAssetXML* getXMLNode(QeAssetXML* source, int length, ...);
	QeAssetXML* getXMLNode(QeAssetXML* source, const char* keys[], int length);
	bool getXMLbValue(bool& output, QeAssetXML& source, int length, ...);
	bool getXMLiValue(int& output, QeAssetXML& source, int length, ...);
	bool getXMLfValue(float& output, QeAssetXML& source, int length, ...);

	QeAssetModel* getModel(const char* _filename, bool bCubeMap=false);
	QeAssetMaterial* getMaterial(const char* _filename);
	QeAssetMaterial* getMaterialImage(const char* _filename, bool bCubeMap=false);
	QeVKImage* getImage(const char* _filename, bool bCubeMap=false);
	VkShaderModule getShader(const char* _filename);
	QeAssetParticleRule* getParticle(const char* _filename);

	std::vector<char> loadFile(const char* _filePath);
	void imageFillto32bits(std::vector<unsigned char>* data, int bytes);
	std::string combinePath(const char* _filename, QeAssetType dataType);

	void setShader( QeAssetShader& shader, QeAssetXML* shaderData, QeAssetXML* defaultShader);
};