#pragma once
#include "qeheader.h"


class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key) :QeModel(_key), vertexBuffer(eBuffer_vertex_texel), 
		outBuffer(eBuffer_storage_compute_shader_return){}
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
	QeVector3f size;
	int bornTargetTranformOID;

	QeVertex createParticleData();
	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void clear();
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
	virtual void updateDrawCommandBuffer(QeDataDrawCommand* command);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet);
	virtual void update1();
};