#include "qeheader.h"


QeViewport ::~QeViewport() {
	cleanupRender();

	vkDestroySemaphore(VK->device, renderFinishedSemaphore, nullptr);
	renderFinishedSemaphore = VK_NULL_HANDLE;
	vkDestroySemaphore(VK->device, imageAvailableSemaphore, nullptr);
	imageAvailableSemaphore = VK_NULL_HANDLE;
}


void QeViewport::init(QeAssetXML* _property) {

	for (int i = 0; i < MAX_VIEWPORT_NUM; ++i) {
		viewports[i].minDepth = 0.0f;
		viewports[i].maxDepth = 1.0f;
		cameras[i] = OBJMGR->getCamera(i, _property);
	}

	currentNum = 1;

	initPostProcessing();
	bUpdateDrawCommandBuffers = true;
	bRecreateRender = false;
	postprocessingDescriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);
	VK->createSemaphores(imageAvailableSemaphore, renderFinishedSemaphore);
	createRender();

	updateViewport();
}

void QeViewport::updateViewport() {
	
	int width = swapChainExtent.width;
	int height = swapChainExtent.height;
	
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
	viewportState.pViewports = viewports.data();
	viewportState.scissorCount = currentNum;
	viewportState.pScissors = scissors.data();			
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
	bRecreateRender = true;
}

void QeViewport::addNewViewport() {
	
	if (currentNum >= MAX_VIEWPORT_NUM) return;

	++currentNum;
	bRecreateRender = true;
}

void QeViewport::setTargetCamera( int index ) {
	
	--index;
	if (index < currentNum)
		targetCamera = index;
}

QeCamera* QeViewport::getTargetCamera() {
	return cameras[targetCamera];
}

void QeViewport::update(float time) {

	if (bRecreateRender) {
		recreateRender();
		bRecreateRender = false;
	}

	if (bUpdateDrawCommandBuffers) {
		updateDrawCommandBuffers();
		bUpdateDrawCommandBuffers = false;
	}

	drawFrame();
}

void QeViewport::drawFrame() {

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(VK->device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		bRecreateRender = true;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)	LOG("failed to acquire swap chain image!");

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &drawCommandBuffers[imageIndex];

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(VK->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)	LOG("failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(VK->presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)	bRecreateRender = true;

	else if (result != VK_SUCCESS)	LOG("failed to present swap chain image!");

	vkQueueWaitIdle(VK->presentQueue);
}

void QeViewport::createRender() {

	VK->createSwapChain(VK->surface, swapChain, swapChainExtent, swapChainImageFormat, swapChainImages, swapChainImageViews);
	renderPass = VK->createRenderPass( swapChainImageFormat );
	VK->createSceneDepthImage(sceneImage, depthImage, swapChainExtent);
	VK->createFramebuffers(framebuffers, sceneImage, depthImage, swapChainImageViews, swapChainExtent, renderPass);
	VK->createDrawCommandBuffers(drawCommandBuffers, swapChainImages.size());
}

void QeViewport::cleanupRender() {

	sceneImage.~QeVKImageBuffer();
	depthImage.~QeVKImageBuffer();

	vkDestroyPipeline(VK->device, postprocessingPipeline, nullptr);
	postprocessingPipeline = VK_NULL_HANDLE;

	for (size_t i = 0; i < framebuffers.size(); i++) {
		vkDestroyFramebuffer(VK->device, framebuffers[i], nullptr);
		framebuffers[i] = VK_NULL_HANDLE;
		vkDestroyImageView(VK->device, swapChainImageViews[i], nullptr);
		swapChainImageViews[i] = VK_NULL_HANDLE;
	}
	vkFreeCommandBuffers(VK->device, VK->commandPool, static_cast<uint32_t>(drawCommandBuffers.size()), drawCommandBuffers.data());
	drawCommandBuffers.clear();
	bUpdateDrawCommandBuffers = true;
	vkDestroyRenderPass(VK->device, renderPass, nullptr);
	renderPass = VK_NULL_HANDLE;
	vkDestroySwapchainKHR(VK->device, swapChain, nullptr);
	swapChain = VK_NULL_HANDLE;
	if (OBJMGR != nullptr)	OBJMGR->cleanupPipeline();
}

void QeViewport::recreateRender() {
	vkDeviceWaitIdle(VK->device);

	cleanupRender();
	createRender();

	updateViewport();
	OBJMGR->recreatePipeline();
	updatePostProcessing();
}

void QeViewport::updateDrawCommandBuffers() {

	for (size_t i = 0; i < drawCommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

		vkBeginCommandBuffer(drawCommandBuffers[i], &beginInfo);
		//vkResetCommandBuffer(drawCommandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		std::array<VkClearValue, 3> clearValues = {};
		if (QE->currentActivity != nullptr) {
			clearValues[0].color = { QE->currentActivity->ambientColor.x, QE->currentActivity->ambientColor.y, QE->currentActivity->ambientColor.z, 1.0f };
			clearValues[2].color = { QE->currentActivity->ambientColor.x, QE->currentActivity->ambientColor.y, QE->currentActivity->ambientColor.z, 1.0f };
		}
		else {
			clearValues[0].color = { 0, 0.5f, 0.5f, 1.0f };
			clearValues[2].color = { QE->currentActivity->ambientColor.x, QE->currentActivity->ambientColor.y, QE->currentActivity->ambientColor.z, 1.0f };
		}
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(drawCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(drawCommandBuffers[i], 0, VP->currentNum, VP->viewports.data());
		vkCmdSetScissor(drawCommandBuffers[i], 0, VP->currentNum, VP->scissors.data());
		vkCmdSetLineWidth(drawCommandBuffers[i], 1.0f);

		OBJMGR->updateDrawCommandBuffer(drawCommandBuffers[i]);

		vkCmdNextSubpass(drawCommandBuffers[i], VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &postprocessingDescriptorSet, 0, nullptr);
		vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, postprocessingPipeline);
		vkCmdDraw(drawCommandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(drawCommandBuffers[i]);
		if (vkEndCommandBuffer(drawCommandBuffers[i]) != VK_SUCCESS)	LOG("failed to record command buffer!");
	}
}

void QeViewport::initPostProcessing() {

	pPostProcessingVert = nullptr;
	pPostProcessingGeom = nullptr;
	pPostProcessingFrag = nullptr;

	if (postprocessingPipeline != VK_NULL_HANDLE) 	vkDestroyPipeline(VK->device, postprocessingPipeline, nullptr);
	postprocessingPipeline = VK_NULL_HANDLE;
}

void QeViewport::updatePostProcessing() {

	QeDataDescriptorSet data;
	data.inputAttachImageViews = sceneImage.view;
	VK->updateDescriptorSet(data, postprocessingDescriptorSet);

	if (postprocessingPipeline == VK_NULL_HANDLE)
		postprocessingPipeline = VK->createPipeline(&pPostProcessingVert->shader, &pPostProcessingGeom->shader, &pPostProcessingFrag->shader, FALSE, FALSE, FALSE, 1);
}