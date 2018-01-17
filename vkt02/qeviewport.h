#pragma once

#include "qeheader.h"

class QeViewport
{
public:
	QeViewport(QeGlobalKey& _key) {}
	~QeViewport() {}

	int currentNum = 0;
	int targetCamera = 0;
	std::array<VkViewport, MAX_VIEWPORT_NUM> viewports;
	std::array<VkRect2D, MAX_VIEWPORT_NUM>	scissors;
	std::array<QeCamera*, MAX_VIEWPORT_NUM> cameras;
	VkPipelineViewportStateCreateInfo viewportState;

	void init(QeAssetXML* _property);
	void addNewViewport();
	void popViewport();
	void updateViewport();
	void update(float time) {}
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();
};

