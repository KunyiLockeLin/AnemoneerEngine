#include "qeviewport.h"


void QeViewport::updateViewport() {
	
	int width1, height1;
	WIN->getWindowSize(width1,height1);
	int width = VLK->swapChainExtent.width;
	int height = VLK->swapChainExtent.height;
	
	int size = int(viewports.size());
	int xNum;
	int yNum;
	float eWidth;
	float eHeight;

	if (width > height) {
		int time1 = width / height;
		int time2 = int(ceil(float(size) / time1));

		if (time2 == 0 ) {
			xNum = size;
			yNum = time2;
		}
		else {
			xNum = time1*time2;
			yNum = time2;
		}
	}
	else
	{
		int time1 = height / width;
		int time2 = int(ceil(float(size) / time1));
		
		if (time2 == 0) {
			yNum = size;
			xNum = time2;
		}
		else {
			yNum = time1*time2;
			xNum = time2;
		}
	}
	eWidth = float(width) / xNum;
	eHeight = float(height)/ yNum;

	for (int i = 0; i < size; ++i) {

		viewports[i].x = (i%xNum)*eWidth;
		viewports[i].y = (i / xNum)*eHeight;
		viewports[i].height = eHeight;

		if((i+1) != size)
			viewports[i].width = eWidth;
		else
			viewports[i].width = width- viewports[i].x;
	}

	size = int(scissors.size());
	for (int i = 0; i<size; ++i)
		scissors[i].extent = VLK->swapChainExtent;

	viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
	viewportState.pViewports = viewports.data();
	viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
	viewportState.pScissors = scissors.data();

	size = int(cameras.size());
	for (int i = 0; i<size ; ++i )
			cameras[i]->faspect = viewports[i].width / viewports[i].height;
}

void QeViewport::popViewport() {
	
	viewports.pop_back();
	scissors.pop_back();
	cameras.pop_back();

	updateViewport();
}

void QeViewport::addNewViewport() {
	
	VkViewport viewport = {};
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	viewports.push_back(viewport);

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = VLK->swapChainExtent;
	scissors.push_back(scissor);

	QeCamera* camera = OBJMGR->getCamera();
	camera->init();
	cameras.push_back(camera);

	updateViewport();

	//VkPipelineViewportStateCreateInfo viewportState =
	//	vks::initializers::pipelineViewportStateCreateInfo(2, 2, 0);
}
