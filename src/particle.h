#pragma once
#include "header.h"

class QeParticle : public QeModel {
   public:
    QeParticle(AeObjectManagerKey &_key)
        : QeModel(_key),
          vertexBuffer(eBuffer_vertex_texel),
          outBuffer(eBuffer_storage_compute_shader_return),
          descriptorSetCompute(eDescriptorSetLayout_Compute) {}
    //~QeParticle() {}

    // QeAssetShader shader;
    QeAssetParticleRule *particleRule;
    int totalParticlesSize;
    int currentParticlesSize;
    std::vector<QeVertex> particles;
    std::vector<int> bDeaths;
    QeVKBuffer vertexBuffer;
    QeVKBuffer outBuffer;
    QeTimer periodTimer;
    QeVector3f size;
    int bornTargetTranformOID;

    QeDataDescriptorSet descriptorSetCompute;
    QeDataComputePipeline computePipeline;

    QeVertex createParticleData();
    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void clear();
    virtual QeDataDescriptorSetModel createDescriptorSetModel();
    virtual void updateDrawCommandBuffer(QeDataDrawCommand *command);
    virtual void updateComputeCommandBuffer(VkCommandBuffer &commandBuffer);
    virtual void update1();
};
