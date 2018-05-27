#pragma once

#include "qeheader.h"

class QeBase
{
public:
	uint16_t id;
	virtual void init( QeAssetXML* _property) {}
	virtual void createPipeline() {}
	virtual void cleanupPipeline() {}
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {}
	virtual void updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {}
	virtual void updateRender(float time) {}
	virtual void updateCompute(float time) {}
};

