#include "qeheader.h"


void QeViewport::init(QeAssetXML* _property) {

	for (int i = 0; i < MAX_VIEWPORT_NUM; ++i) {
		viewports[i].minDepth = 0.0f;
		viewports[i].maxDepth = 1.0f;
		cameras[i] = OBJMGR->getCamera(i, _property);
	}

	currentNum = 1;
	updateViewport();
}

void QeViewport::updateViewport() {
	
	int width = VK->swapChainExtent.width;
	int height = VK->swapChainExtent.height;
	
	int xNum;
	int yNum;
	float eWidth;
	float eHeight;
	int time1;
	int time2;
	if (width > height) {
		time1 = width / height;
		time2 = int(round(sqrtf(float(currentNum) / time1)));

		xNum = int(ceil(float(currentNum) /time2));
		yNum = time2;
	}
	else
	{
		time1 = height / width;
		time2 = int(round(sqrtf(float(currentNum) / time1)));
		
		yNum = int(ceil(float(currentNum) / time2));
		xNum = time2;
	}
	eWidth = float(width) / xNum;
	eHeight = float(height)/ yNum;

	for (int i = 0; i < currentNum; ++i) {

		viewports[i].x = (i%xNum)*eWidth;
		viewports[i].y = (i / xNum)*eHeight;
		viewports[i].height = eHeight;

		if((i+1) != currentNum)
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
	viewportState.viewportCount = currentNum;
	//viewportState.pViewports = viewports.data();
	viewportState.scissorCount = currentNum;
	//viewportState.pScissors = scissors.data();			
}

void QeViewport::popViewport() {
	
	if (currentNum < 2) return;
	
	--currentNum;
	targetCamera = 0;
	/*viewports[currentNum].x = 0;
	viewports[currentNum].y = 0;
	viewports[currentNum].height = 0;
	viewports[currentNum].width = 0;
	scissors[currentNum].extent.width = 0;
	scissors[currentNum].extent.height = 0;
	scissors[currentNum].offset.x = 0;
	scissors[currentNum].offset.y = 0;
	*/
	VK->bRecreateRender = true;
}

void QeViewport::addNewViewport() {
	
	if (currentNum >= MAX_VIEWPORT_NUM) return;

	++currentNum;
	VK->bRecreateRender = true;
}

void QeViewport::setTargetCamera( int index ) {
	
	--index;
	if (index < currentNum)
		targetCamera = index;
}

QeCamera* QeViewport::getTargetCamera() {
	return cameras[targetCamera];
}