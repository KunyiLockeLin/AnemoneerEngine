#pragma once

#include "header.h"

struct QeVertex {
    AeVector<float, 4> pos = {0.0f, 0.0f, 0.0f, 1.0f};
    AeVector<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f};
    AeVector<float, 4> uv;
    AeVector<float, 4> normal = {0.0f, 0.0f, 0.0f, 1.0f};
    AeVector<float, 4> tangent;
    AeVector<float, 4> joint;
    AeVector<float, 4> weight;

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
    std::vector<AeVector<float, 3>> translationOutput;
    std::vector<float> rotationInput;
    std::vector<AeVector<float, 4>> rotationOutput;
    // std::vector<float> scaleInput;
    // std::vector<QeVector3f> scaleOutput;
};

struct QeAssetModel {
    std::vector<QeVertex> vertices;
    std::vector<uint32_t> indices;
    size_t indexSize = 0;

    QeVKBuffer vertex;
    QeVKBuffer index;
    AeVector<float, 3> scale;

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
    AeVector<float, 4> baseColor;
    AeVector<float, 4> metallicRoughnessEmissive;
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

class QeGameAsset {
    SINGLETON_CLASS(QeGameAsset)

   public:
    std::map<std::string, QeAssetModel *> astModels;
    std::map<std::string, QeAssetMaterial *> astMaterials;
    std::map<std::string, VkShaderModule> astShaders;
    std::map<std::string, QeVKImage *> astTextures;
    //std::map<int, QeAssetParticleRule *> astParticles;

    ~QeGameAsset();

    AeXMLNode *getXMLEditNode(AE_GAMEOBJECT_TYPE _type, int eid);

    QeAssetModel *getModel(const char *_filename, bool bCubeMap = false, float *param = nullptr);
    // QeAssetMaterial* getMaterial(const char* _filename);
    QeAssetMaterial *getMaterialImage(const char *_filename, bool bCubeMap = false);
    QeVKImage *getImage(const char *_filename, bool bCubeMap = false, bool bGamma = false);
    VkShaderModule getShader(const char *_filename);
    //QeAssetParticleRule *getParticle(int eid);

    void imageFillto32bits(std::vector<unsigned char> *data, int bytes);
    std::string combinePath(const char *_filename, QeGameAssetType dataType);

    void setGraphicsShader(QeAssetGraphicsShader &shader, AeXMLNode *shaderData, const char *defaultShaderType);
};
#define G_AST QeGameAsset::getInstance()
