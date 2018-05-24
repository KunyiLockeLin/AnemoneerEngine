#pragma once

#include "qeheader.h"

class QeCompute
{
public:
	QeCompute(QeObjectMangerKey& _key) {}
	~QeCompute();

	VkShaderModule comp = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;

};

