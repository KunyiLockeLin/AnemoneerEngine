#pragma once

#include "header.h"

struct QeDataModel {
    QeMatrix4x4f model;
    QeMatrix4x4f joints[MAX_JOINT_NUM];
    QeVector4f param1;  // 0: bColorMap, 1: bCubeMap, 2: bNormalMap, 3:
                        // bMetallicRoughnessMap
    QeVector4f param2;  // 0: outlineWidth, 1: vertexNum
    QeDataMaterial material;
};

class QeModel : public QeComponent {
   public:
    QeModel(AeObjectManagerKey &_key) : QeComponent(_key), modelBuffer(eBuffer_uniform), descriptorSet(eDescriptorSetLayout_Model) {}
    //~QeModel(){}

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();
    virtual void update1();
    virtual void update2();

    AeGameObjectComponentModelData data;
    // QeMaterial, QeAnimation
    const char *shaderKey;
    QeAssetModel *modelData = nullptr;
    QeAssetMaterial *materialData = nullptr;
    QeAssetGraphicsShader graphicsShader;
    QeAssetGraphicsShader normalShader;
    QeAssetGraphicsShader outlineShader;
    QeDataGraphicsPipeline graphicsPipeline;

    QeDataDescriptorSet descriptorSet;

    QeDataModel bufferData;
    QeVKBuffer modelBuffer;
    bool bUpdateMaterialOID;
    bool bRotate = true;
    bool b2D = false;

    virtual QeDataDescriptorSetModel createDescriptorSetModel();
    virtual bool isShowByCulling(QeCamera *camera);

    virtual void updateDrawCommandBuffer(QeDataDrawCommand *command);
    virtual void updateComputeCommandBuffer(VkCommandBuffer &commandBuffer) {}
};
