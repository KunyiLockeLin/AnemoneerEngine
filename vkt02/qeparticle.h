#pragma once
#include "qeheader.h"


class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key) :QeModel(_key, eModel_Particle) {}
	~QeParticle() {}

	uint16_t eid;
	QeAssetShader shader;
	QeAssetParticleRule* particleRule;
	uint16_t particlesSize;
	std::vector<QeVertex> particles;
	QeVKBuffer uboParticleRule;
	QeVKBuffer VertexBuffer;
	bool bFollow=true;
	
	virtual void init(QeAssetXML* _property);
	virtual void createPipeline();
	virtual void setMatModel();
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer);
	virtual void updateCompute(float time);
};

