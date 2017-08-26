#include "qeviewport.h"



QeViewport::QeViewport()
{
}


QeViewport::~QeViewport()
{
}

void QeViewport::init() {
	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)VLK->swapChainExtent.width;
	viewport.height = (float)VLK->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = VLK->swapChainExtent;

	viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//VkPipelineViewportStateCreateInfo viewportState =
	//	vks::initializers::pipelineViewportStateCreateInfo(2, 2, 0);
}
