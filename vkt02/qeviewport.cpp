#include "qeheader.h"


QeViewport ::~QeViewport() {
	cleanupRender();

	for (size_t i = 0; i < VK->MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(VK->device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(VK->device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(VK->device, inFlightFences[i], nullptr);
	}
	renderFinishedSemaphores.clear();
	imageAvailableSemaphores.clear();
	inFlightFences.clear();
}


void QeViewport::init(QeAssetXML* _property) {

	initProperty = _property;

	bUpdateDrawCommandBuffers = true;
	bRecreateRender = true;
	AST->setShader(shader, AST->getXMLNode(initProperty, 1, "postprocessing"), AST->getXMLNode(3, AST->CONFIG, "defaultShader", "postprocessing"));
	VK->createDescriptorSet(postprocessingDescriptorSet);
	VK->createSyncObjects(imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
}

void QeViewport::updateViewport() {
	
	int width = swapChainExtent.width;
	int height = swapChainExtent.height;
	size_t currentNum = viewports.size();

	if (currentNum == 0) {
		addNewViewport();
		++currentNum;
	}

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

		viewports[i]->viewport.minDepth = 0.0f;
		viewports[i]->viewport.maxDepth = 1.0f;
		viewports[i]->viewport.x = (i%xNum)*eWidth;
		viewports[i]->viewport.y = (i / xNum)*eHeight;
		viewports[i]->viewport.height = eHeight;

		if((i+1) != currentNum)
			viewports[i]->viewport.width = eWidth;
		else
			viewports[i]->viewport.width = width- viewports[i]->viewport.x;

		viewports[i]->scissor.extent.width = int(viewports[i]->viewport.width);
		viewports[i]->scissor.extent.height = int(viewports[i]->viewport.height);
		viewports[i]->scissor.offset.x = int(viewports[i]->viewport.x);
		viewports[i]->scissor.offset.y = int(viewports[i]->viewport.y);
		viewports[i]->camera->faspect = viewports[i]->viewport.width / viewports[i]->viewport.height;
		viewports[i]->camera->bUpdateBuffer = true;
	}
}

void QeViewport::popViewport() {

	size_t size = viewports.size();
	if (size == 1) return;

	delete viewports[size - 1];
	viewports.pop_back();

	setTargetCamera(0);
	bRecreateRender = true;
}

void QeViewport::addNewViewport() {

	QeDataViewport* vp = new QeDataViewport();
	viewports.push_back(vp);

	vp->lights.push_back(ACT->light);
	VK->createBuffer(vp->lightsBuffer, sizeof(vp->lights[0]->bufferData), nullptr);

	QeAssetXML*node = AST->getXMLNode(initProperty, 1, "cameras");
	if (node == nullptr || node->nexts.size() == 0) {
		node = AST->getXMLNode(2, AST->CONFIG, "defaultCamera");
		VP->init(node);
	}
	else node = node->nexts[0];

	uint16_t id = atoi(AST->getXMLValue(node, 1, "id"));
	vp->camera = OBJMGR->getCamera(id+ int(viewports.size()), initProperty);

	VK->createBuffer(vp->environmentBuffer, sizeof(vp->environmentData), nullptr);
	
	QeDataDescriptorSetCommon data;
	data.environmentBuffer = vp->environmentBuffer.buffer;
	data.lightsBuffer = vp->lightsBuffer.buffer;

	VK->createDescriptorSet(vp->commonDescriptorSet);
	VK->updateDescriptorSet(&data, vp->commonDescriptorSet);

	bRecreateRender = true;
}

void QeViewport::setTargetCamera( int index ) {
	
	if (index < viewports.size() && index != currentTargetViewport ) {
		currentTargetViewport = index;
		getTargetCamera()->updateAxis();
	}
}

QeCamera* QeViewport::getTargetCamera() {
	return viewports[currentTargetViewport]->camera;
}

void QeViewport::updateBuffer() {

	bool b = false;
	size_t size = viewports.size();
	for (size_t i = 0; i<size ;++i) {
		if (viewports[i]->camera->bUpdateBuffer) {
			viewports[i]->camera->bUpdateBuffer = false;

			viewports[i]->environmentData.ambintColor = ACT->ambientColor;
			viewports[i]->environmentData.camera = viewports[i]->camera->bufferData;
			viewports[i]->environmentData.param.x = float(viewports[i]->lights.size());

			VK->setMemoryBuffer(viewports[i]->environmentBuffer, sizeof(viewports[i]->environmentData), &viewports[i]->environmentData);
		}
		size_t size1 = viewports[i]->lights.size();
		for (size_t j = 0; j<size1 ; ++j) {
			if (viewports[i]->lights[j]->bUpdateBuffer) {
				b = true;
				std::vector<QeDataLight> data;
				data.resize(size1);
				for (size_t j = 0; j < size1; ++j) {
					data[j]= viewports[i]->lights[j]->bufferData;
				}
				VK->setMemoryBuffer(viewports[i]->lightsBuffer, sizeof(data[0])*size1, data.data());
			}
		}
	}

	if (b) {
		size = viewports.size();
		for (size_t i = 0; i < size; ++i) {
			size_t size1 = viewports[i]->lights.size();
			for (size_t j = 0; j < size1; ++j) {
				viewports[i]->lights[j]->bUpdateBuffer = false;
			}
		}
	}
}

void QeViewport::updateCompute(float time) {
	if (bRecreateRender) {
		recreateRender();
		bRecreateRender = false;
	}
	VK->pushConstants[0] = time;
	bUpdateDrawCommandBuffers = true;
}

void QeViewport::updateRender(float time) {

	if (bRecreateRender) return;
	updateBuffer();

	if (bUpdateDrawCommandBuffers) {
		updateDrawCommandBuffers();
		bUpdateDrawCommandBuffers = false;
	}
	drawFrame();
}

void QeViewport::drawFrame() {

	vkWaitForFences(VK->device, 1, &inFlightFences[currentFrame], VK_TRUE, 2000000000);
	vkResetFences(VK->device, 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(VK->device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		bRecreateRender = true;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)	LOG("failed to acquire swap chain image! "+ result);

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &drawCommandBuffers[imageIndex];
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkQueueSubmit(VK->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
	if( result != VK_SUCCESS)	LOG("failed to submit draw command buffer! " + result);

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

	currentFrame = (++currentFrame) % VK->MAX_FRAMES_IN_FLIGHT;
}

void QeViewport::createRender() {

	VK->createSwapChain(VK->surface, swapChain, swapChainExtent, swapChainImageFormat, swapChainImages);
	renderPass = VK->createRenderPass(swapChainImageFormat );
	VK->createPresentDepthImage(presentImage, depthImage, swapChainExtent);
	VK->createFramebuffers(swapChainFramebuffers, presentImage, depthImage, swapChainImages, swapChainExtent, renderPass);
	VK->createCommandBuffers(drawCommandBuffers, swapChainImages.size());

	mainViewport.minDepth = 0.0f;
	mainViewport.maxDepth = 1.0f;
	mainViewport.x = 0.0f;
	mainViewport.y = 0.0f;
	mainViewport.height = float(swapChainExtent.height);
	mainViewport.width = float(swapChainExtent.width);

	mainScissor.offset.x = int(mainViewport.x);
	mainScissor.offset.y = int(mainViewport.y);
	mainScissor.extent.height = uint32_t(mainViewport.height);
	mainScissor.extent.width = uint32_t(mainViewport.width);
}

void QeViewport::cleanupRender() {

	presentImage.~QeVKImage();
	depthImage.~QeVKImage();

	//vkDestroyPipeline(VK->device, postprocessingPipeline, nullptr);
	//postprocessingPipeline = VK_NULL_HANDLE;
	size_t size, i;
	
	postprocessingPipeline = nullptr;

	size = swapChainFramebuffers.size();
	for ( i = 0; i < size; i++) {
		vkDestroyFramebuffer(VK->device, swapChainFramebuffers[i], nullptr);
		swapChainFramebuffers[i] = VK_NULL_HANDLE;
		//vkDestroyImageView(VK->device, swapChainImageViews[i], nullptr);
		//swapChainImageViews[i] = VK_NULL_HANDLE;
		vkDestroyImageView(VK->device, swapChainImages[i].view, nullptr);
		swapChainImages[i].view = VK_NULL_HANDLE;
		swapChainImages[i].image = VK_NULL_HANDLE;
	}
	swapChainFramebuffers.clear();

	vkFreeCommandBuffers(VK->device, VK->commandPool, static_cast<uint32_t>(drawCommandBuffers.size()), drawCommandBuffers.data());
	drawCommandBuffers.clear();
	
	if (renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(VK->device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}
	
	if (swapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(VK->device, swapChain, nullptr);
		swapChain = VK_NULL_HANDLE;
	}

	for (size_t i = 0;i<VK->graphicsPipelines.size();++i ) {
		vkDestroyPipeline(VK->device, VK->graphicsPipelines[i]->graphicsPipeline, nullptr);
		delete VK->graphicsPipelines[i];
	}
	VK->graphicsPipelines.clear();

	if (OBJMGR != nullptr)	OBJMGR->cleanupPipeline();
}

void QeViewport::recreateRender() {
	vkDeviceWaitIdle(VK->device);

	cleanupRender();
	createRender();

	updateViewport();

	QeDataDescriptorSetPostprocessing data;
	data.inputAttachImageViews = presentImage.view;
	VK->updateDescriptorSet(&data, postprocessingDescriptorSet);
	postprocessingPipeline = VK->createGraphicsPipeline(&shader, eGraphicsPipeLine_Postprogessing, false, 1);

	OBJMGR->recreatePipeline();
}


/*void QeViewport::updateComputeCommandBuffers() {
	for (size_t i = 0; i < computeCommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(computeCommandBuffers[i], &beginInfo);
		//vkResetCommandBuffer(drawCommandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		// pushConstants
		VK->updatePushConstnats(computeCommandBuffers[i]);

		//compute shader
		OBJMGR->updateComputeCommandBuffer(computeCommandBuffers[i]);

		if (vkEndCommandBuffer(computeCommandBuffers[i]) != VK_SUCCESS)	LOG("failed to record command buffer!");
	}

}*/

void QeViewport::updateDrawCommandBuffers() {

	size_t size = drawCommandBuffers.size();
	for (size_t i = 0; i < size; i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(drawCommandBuffers[i], &beginInfo);
		//vkResetCommandBuffer(drawCommandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		// pushConstants
		VK->updatePushConstnats(drawCommandBuffers[i]);

		//compute shader
		OBJMGR->updateComputeCommandBuffer(drawCommandBuffers[i]);

		// graphics shader
		std::array<VkClearValue, 3> clearValues = {};
		if (ACT != nullptr) {
			clearValues[0].color = { ACT->ambientColor.x, ACT->ambientColor.y, ACT->ambientColor.z, 1.0f };
			clearValues[2].color = { ACT->ambientColor.x, ACT->ambientColor.y, ACT->ambientColor.z, 1.0f };
		}
		else {
			clearValues[0].color = { 0, 0.5f, 0.5f, 1.0f };
			clearValues[2].color = { 0, 0.5f, 0.5f, 1.0f };
		}
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(drawCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		size_t size1 = viewports.size();
		for (currentCommandViewport = 0; currentCommandViewport<size1 ; ++currentCommandViewport) {
			vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewports[currentCommandViewport]->viewport);
			vkCmdSetScissor(drawCommandBuffers[i], 0, 1, &viewports[currentCommandViewport]->scissor);
			vkCmdSetLineWidth(drawCommandBuffers[i], 2.0f);

			OBJMGR->updateDrawCommandBuffer(drawCommandBuffers[i]);
		}
		vkCmdNextSubpass(drawCommandBuffers[i], VK_SUBPASS_CONTENTS_INLINE);
		vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &mainViewport);
		vkCmdSetScissor(drawCommandBuffers[i], 0, 1, &mainScissor);
		vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 2, 1, &postprocessingDescriptorSet.descriptorSet, 0, nullptr);
		vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, postprocessingPipeline->graphicsPipeline);
		vkCmdDraw(drawCommandBuffers[i], 1, 1, 0, 0);
		
		vkCmdEndRenderPass(drawCommandBuffers[i]);
		if (vkEndCommandBuffer(drawCommandBuffers[i]) != VK_SUCCESS)	LOG("failed to record command buffer!");
	}
}