#pragma once

#include "qeheader.h"

class QeViewport
{
private:
public:
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportState;

public:
	QeViewport();
	~QeViewport();
	void init();

	void update(float time) {}
};

