#pragma once

#include "qeheader.h"

class QeViewport
{
public:
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportState;

	QeViewport();
	~QeViewport();
	void init();

	void update(float time) {}
};

