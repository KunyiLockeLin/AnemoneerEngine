#pragma once

#include "qeheader.h"

// MAX number of viewport is 8.

class QeViewport
{
public:
	QeViewport(QeGlobalKey& _key) {}
	~QeViewport() {}

	std::vector<VkViewport> viewports;
	std::vector<VkRect2D> scissors;
	std::vector<QeCamera*> cameras;
	VkPipelineViewportStateCreateInfo viewportState;

	void addNewViewport();
	void popViewport();
	void updateViewport();
	void update(float time) {}
};

