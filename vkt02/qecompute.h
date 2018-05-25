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


	void init();
};

