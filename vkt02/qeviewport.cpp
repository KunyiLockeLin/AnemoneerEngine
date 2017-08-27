#include "qeviewport.h"


void QeViewport::updateViewport() {
	
	int width = VLK->swapChainExtent.width;
	int height = VLK->swapChainExtent.height;
	
	int size = int(viewports.size());
	int xNum;
	int yNum;
	float eWidth;
	float eHeight;
	int time1;
	int time2;
	if (width > height) {
		time1 = width / height;
		time2 = int(ceil(sqrtf(float(size) / time1)));

		xNum = int(ceil(float(size) /time2));
		yNum = time2;
	}
	else
	{
		time1 = height / width;
		time2 = int(ceil(sqrtf(float(size) / time1)));
		
		yNum = int(ceil(float(size) / time2));
		xNum = time2;
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

		scissors[i].extent.width = int(viewports[i].width);
		scissors[i].extent.height = int(viewports[i].height);
		scissors[i].offset.x = int(viewports[i].x);
		scissors[i].offset.y = int(viewports[i].y);
		cameras[i]->faspect = viewports[i].width / viewports[i].height;
	}

	viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
	//viewportState.pViewports = viewports.data();
	viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
	//viewportState.pScissors = scissors.data();			
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
	scissors.push_back(scissor);

	QeCamera* camera = OBJMGR->getCamera();
	camera->init();
	cameras.push_back(camera);

	updateViewport();
}
