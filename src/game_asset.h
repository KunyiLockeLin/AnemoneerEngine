#pragma once

#include "header.h"

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

class QeGameAsset {
    SINGLETON_CLASS(QeGameAsset)

   public:
    std::map<std::string, QeAssetModel *> astModels;
    std::map<std::string, QeAssetMaterial *> astMaterials;
    std::map<std::string, VkShaderModule> astShaders;
    std::map<std::string, QeVKImage *> astTextures;
    std::map<int, QeAssetParticleRule *> astParticles;

    ~QeGameAsset();

    QeAssetXML *getXMLEditNode(QeComponentType _type, int eid);

    QeAssetModel *getModel(const char *_filename, bool bCubeMap = false, float *param = nullptr);
    // QeAssetMaterial* getMaterial(const char* _filename);
    QeAssetMaterial *getMaterialImage(const char *_filename, bool bCubeMap = false);
    QeVKImage *getImage(const char *_filename, bool bCubeMap = false, bool bGamma = false);
    VkShaderModule getShader(const char *_filename);
    QeAssetParticleRule *getParticle(int eid);

    void imageFillto32bits(std::vector<unsigned char> *data, int bytes);
    std::string combinePath(const char *_filename, QeGameAssetType dataType);

    void setGraphicsShader(QeAssetGraphicsShader &shader, QeAssetXML *shaderData, const char *defaultShaderType);
};
#define G_AST QeGameAsset::getInstance()
