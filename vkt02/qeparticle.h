#pragma once
#include "qeheader.h"


class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key, QeObjectType _type = eObject_Particle) :QeModel(_key, _type), 
				vertexBuffer(eBuffer_vertex_texel), outBuffer(eBuffer_storage_compute_shader_return){}
	//~QeParticle() {}

	QeAssetShader shader;
	QeAssetParticleRule* particleRule;
	uint32_t particlesSize;
	std::vector<QeVertex> particles;
	std::vector<int> bDeaths;
	QeVKBuffer vertexBuffer;
	QeVKBuffer outBuffer;

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
	virtual void setMatModel();
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer);
	virtual void updateCompute(float time);
};