#pragma once

#include "header.h"

struct QeDataModel {
    QeMatrix4x4f model;
    QeMatrix4x4f joints[MAX_JOINT_NUM];
    AeVector<float,4> param1;  // 0: bColorMap, 1: bCubeMap, 2: bNormalMap, 3:
                        // bMetallicRoughnessMap
    AeVector<float, 4> param2;  // 0: outlineWidth, 1: vertexNum
    QeDataMaterial material;
};

class QeModel : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE_INITIALIZE(QeModel, AeGameObjectComponentModelData, modelBuffer(eBuffer_uniform),
                                       descriptorSet(eDescriptorSetLayout_Model))

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();
    virtual void updatePreRender();
    virtual void updatePostRender();

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
