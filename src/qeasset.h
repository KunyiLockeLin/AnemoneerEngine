#pragma once

#include "qeheader.h"

struct QeVertex {
    QeVector4f pos = {0.0f, 0.0f, 0.0f, 1.0f};
    QeVector4f color = {0.0f, 0.0f, 0.0f, 1.0f};
    QeVector4f uv;
    QeVector4f normal = {0.0f, 0.0f, 0.0f, 1.0f};
    QeVector4f tangent;
    QeVector4f joint;
    QeVector4f weight;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions();
    bool operator==(const QeVertex &other) const;
};

struct QeDataJoint {
    unsigned char id = 0;
    std::vector<QeDataJoint *> children;
    const char *name = nullptr;
    // QeVector3f translation;
    // QeVector4f rotation;
    // QeVector3f scale;
    // QeMatrix4x4f transform;
    QeMatrix4x4f inverseBindMatrix;
    std::vector<float> translationInput;
    std::vector<QeVector3f> translationOutput;
    std::vector<float> rotationInput;
    std::vector<QeVector4f> rotationOutput;
    // std::vector<float> scaleInput;
    // std::vector<QeVector3f> scaleOutput;
};

struct QeAssetModel {
    std::vector<QeVertex> vertices;
    std::vector<uint32_t> indices;
    size_t indexSize = 0;

    QeVKBuffer vertex;
    QeVKBuffer index;
    QeVector3f scale;

    QeAssetMaterial *pMaterial = nullptr;
    QeDataJoint *rootJoint = nullptr;
    std::vector<QeDataJoint> jointsAnimation;
    unsigned char animationNum = 0;
    std::vector<unsigned int> animationStartFrames;
    std::vector<unsigned int> animationEndFrames;

    QeAssetModel() : vertex(eBuffer_vertex), index(eBuffer_index) {}
    ~QeAssetModel();
};

/*enum QeMaterialType {
        eMaterialPhong,
        eMaterialPBR,
};

struct QeDataMaterialPhong {
        QeVector4f diffuse;
        QeVector4f ambient;
        QeVector4f specular;
        QeVector4f emissive;
        QeVector4f param; // 1: specularExponent(1f), 2: refraction(1f), 3:
alpha(1f),
};

struct QeDataMaterialPBR {
        QeVector4f baseColor;
        QeVector4f metallicRoughness;
        QeVector4f emissive;
};*/

struct QeDataMaterial {
    /*union{
            QeDataMaterialPhong phong;
            QeDataMaterialPBR pbr;
    };*/
    QeVector4f baseColor;
    QeVector4f metallicRoughnessEmissive;
    QeDataMaterial() {}
};

struct QeAssetImage {
    // QeVKImage* pDiffuseMap = nullptr; // baseColorMap;
    QeVKImage *pBaseColorMap = nullptr;
    QeVKImage *pCubeMap = nullptr;
    QeVKImage *pNormalMap = nullptr;
    QeVKImage *pMetallicRoughnessMap = nullptr;
};

struct QeAssetGraphicsShader {
    VkShaderModule vert = VK_NULL_HANDLE;
    VkShaderModule tesc = VK_NULL_HANDLE;
    VkShaderModule tese = VK_NULL_HANDLE;
    VkShaderModule geom = VK_NULL_HANDLE;
    VkShaderModule frag = VK_NULL_HANDLE;
};

struct QeAssetMaterial {
    // QeMaterialType type = eMaterialPhong;
    QeAssetImage image;
    QeDataMaterial value;

    QeAssetMaterial() {}
};

struct QeAssetParticleRule {
    const char *image = nullptr;
    bool bAlpha, bReborn;
    int count_once, count_period, count_total, count_range;
    int life_scend, life_range;
    QeVector3f init_pos_square;        // length, width, height
    QeVector3f init_pos_square_range;  // length, width, height
    QeVector2f init_pos_cycle;         // radius, degree
    QeVector2f init_pos_cycle_range;   // radius, degree
    QeVector3f init_speed;
    QeVector3f init_speed_range;
    QeVector3f force;
    QeVector3f force_range;
    QeVector2f size;
    QeVector2f size_range;
    QeVector3f color;
    QeVector3f color_range;
};

struct QeAssetXML {
    std::string key;
    std::string value;
    std::vector<std::string> eKeys;
    std::vector<std::string> eValues;
    std::vector<QeAssetXML *> nexts;
    QeAssetXML *parent = nullptr;

    ~QeAssetXML();
};

struct QeAssetJSON {
    std::vector<std::string> eKeysforValues;
    std::vector<std::string> eValues;
    std::vector<std::string> eKeysforNodes;
    std::vector<QeAssetJSON *> eNodes;
    std::vector<std::string> eKeysforArrayValues;
    std::vector<std::vector<std::string>> eArrayValues;
    std::vector<std::string> eKeysforArrayNodes;
    std::vector<std::vector<QeAssetJSON *>> eArrayNodes;

    ~QeAssetJSON();
};

class QeAsset {
   public:
    const char *CONFIG = "data/config.xml";

    std::map<std::string, QeAssetXML *> astXMLs;
    std::map<std::string, QeAssetJSON *> astJSONs;
    std::map<std::string, QeAssetModel *> astModels;
    std::map<std::string, QeAssetMaterial *> astMaterials;
    std::map<std::string, VkShaderModule> astShaders;
    std::map<std::string, QeVKImage *> astTextures;
    std::map<int, QeAssetParticleRule *> astParticles;

    QeAsset(QeGlobalKey &_key) {}
    ~QeAsset();

    QeAssetJSON *getJSON(const char *_filePath);
    const char *getJSONValue(int length, ...);
    const char *getJSONValue(QeAssetJSON *source, int length, ...);
    const char *getJSONValue(QeAssetJSON *source, const char *keys[], int length);
    QeAssetJSON *getJSONNode(int length, ...);
    QeAssetJSON *getJSONNode(QeAssetJSON *source, int length, ...);
    QeAssetJSON *getJSONNode(QeAssetJSON *source, const char *keys[], int length);
    std::vector<std::string> *getJSONArrayValues(int length, ...);
    std::vector<std::string> *getJSONArrayValues(QeAssetJSON *source, int length, ...);
    std::vector<std::string> *getJSONArrayValues(QeAssetJSON *source, const char *keys[], int length);
    std::vector<QeAssetJSON *> *getJSONArrayNodes(int length, ...);
    std::vector<QeAssetJSON *> *getJSONArrayNodes(QeAssetJSON *source, int length, ...);
    std::vector<QeAssetJSON *> *getJSONArrayNodes(QeAssetJSON *source, const char *keys[], int length);
    bool getJSONbValue(bool *output, QeAssetJSON *source, int length, ...);
    bool getJSONiValue(int *output, QeAssetJSON *source, int length, ...);
    bool getJSONfValue(float *output, QeAssetJSON *source, int length, ...);

    QeAssetXML *getXML(const char *_filePath);
    const char *getXMLValue(int length, ...);
    const char *getXMLValue(QeAssetXML *source, int length, ...);
    const char *getXMLValue(QeAssetXML *source, const char *keys[], int length);
    QeAssetXML *getXMLNode(int length, ...);
    QeAssetXML *getXMLNode(QeAssetXML *source, int length, ...);
    QeAssetXML *getXMLNode(QeAssetXML *source, const char *keys[], int length);
    QeAssetXML *copyXMLNode(QeAssetXML *source);
    void copyXMLValue(QeAssetXML *from, QeAssetXML *to);
    void copyXMLNode(QeAssetXML *from, QeAssetXML *to);
    void addXMLNode(QeAssetXML *source, QeAssetXML *node);
    void setXMLKey(QeAssetXML *source, const char *key);
    void setXMLValue(QeAssetXML *source, const char *value);
    void setXMLValue(QeAssetXML *source, const char *key, const char *value);
    void removeXML(std::string path);
    void removeXMLNode(QeAssetXML *source, QeAssetXML *node);
    void outputXML(QeAssetXML *source, const char *path, int level = 0, std::string *content = nullptr);

    QeAssetXML *getXMLEditNode(QeComponentType _type, int eid);
    bool getXMLbValue(bool *output, QeAssetXML *source, int length, ...);
    bool getXMLiValue(int *output, QeAssetXML *source, int length, ...);
    bool getXMLfValue(float *output, QeAssetXML *source, int length, ...);

    QeAssetModel *getModel(const char *_filename, bool bCubeMap = false, float *param = nullptr);
    // QeAssetMaterial* getMaterial(const char* _filename);
    QeAssetMaterial *getMaterialImage(const char *_filename, bool bCubeMap = false);
    QeVKImage *getImage(const char *_filename, bool bCubeMap = false, bool bGamma = false);
    VkShaderModule getShader(const char *_filename);
    QeAssetParticleRule *getParticle(int eid);

    std::vector<char> loadFile(const char *_filePath);
    void imageFillto32bits(std::vector<unsigned char> *data, int bytes);
    std::string combinePath(const char *_filename, QeAssetType dataType);

    void setGraphicsShader(QeAssetGraphicsShader &shader, QeAssetXML *shaderData, const char *defaultShaderType);
};
