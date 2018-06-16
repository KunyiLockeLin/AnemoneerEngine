#pragma once
#include "qeheader.h"


class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key) :QeModel(_key, eModel_Particle), VertexBuffer(eBuffer_vertex_texel), outBuffer(eBuffer_storage_compute_shader_return){}
	~QeParticle() {}

	uint16_t eid;
	QeAssetShader shader;
	QeAssetParticleRule* particleRule;
	uint32_t particlesSize;
	std::vector<QeVertex> particles;
	std::vector<int> bDeaths;
	//QeVKBuffer uboParticleRule(eBuffer_uniform);
	QeVKBuffer VertexBuffer;
	QeVKBuffer outBuffer;

	bool bFollow=true;
	
	virtual void init(QeAssetXML* _property);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
	virtual void setMatModel();
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer);
	virtual void updateCompute(float time);
};