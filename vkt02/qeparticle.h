#pragma once
#include "qeheader.h"

class QeParticle : public QeModel
{
public:

	QeParticle(QeObjectMangerKey& _key) :QeModel(_key) {}
	/*~QeParticle() {}

	VkShaderModule shader = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	const uint32_t  PARTICLES_COUNT = 2000;
	std::vector<QeVertex> particles;
	QeVKBuffer		VertexBuffer;

	QeTimer timer;

	virtual void init(QeAssetXML* _property);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	virtual void updateCompute(float time);*/
};

