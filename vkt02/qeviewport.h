#pragma once

#include "qeheader.h"

class QeViewport
{
public:
	QeViewport(QeGlobalKey& _key) {}
	~QeViewport() {}

	int currentNum = 1;
	int targetCamera = 1;
	std::array<VkViewport, MAX_VIEWPORT_NUM> viewports;
	std::array<VkRect2D, MAX_VIEWPORT_NUM>	scissors;
	std::array<QeCamera*, MAX_VIEWPORT_NUM> cameras;
	VkPipelineViewportStateCreateInfo viewportState;

	void init();
	void addNewViewport();
	void popViewport();
	void updateViewport();
	void update(float time) {}
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();
};

