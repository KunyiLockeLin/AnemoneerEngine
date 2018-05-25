#pragma once

#include "qeheader.h"

class QeCompute
{
public:
	QeCompute(QeObjectMangerKey& _key) {}
	~QeCompute();

	VkShaderModule shader = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer;

	const uint32_t  PARTICLES_COUNT = 2000;
	QeVKBuffer		VertexBuffer;
	
	void init();
};

