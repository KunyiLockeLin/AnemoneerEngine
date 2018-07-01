#pragma once
#include "qeheader.h"


class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key, QeObjectType _type = eObject_Particle) :QeModel(_key, _type), 
				vertexBuffer(eBuffer_vertex_texel), outBuffer(eBuffer_storage_compute_shader_return){}
	//~QeParticle() {}

	//QeAssetShader shader;
	QeAssetParticleRule* particleRule;
	int  totalParticlesSize;
	int  currentParticlesSize;
	std::vector<QeVertex> particles;
	std::vector<int> bDeaths;
	QeVKBuffer vertexBuffer;
	QeVKBuffer outBuffer;
	QeTimer periodTimer;
	
	QeVertex createParticleData();
	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
	virtual void updateDrawCommandBuffer(QeDataDrawCommand* command);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet);
	virtual void updateCompute(float time);
};