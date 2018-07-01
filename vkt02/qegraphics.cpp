#include "qeheader.h"


QeGraphics ::~QeGraphics() {
	cleanupRender();

	size_t size = fences.size();
	for (size_t i = 0; i < size; i++)
		vkDestroyFence(VK->device, fences[i], nullptr);
	fences.clear();

	vkDestroySemaphore(VK->device, renderCompleteSemaphore, nullptr);
	//renderFinishedSemaphores.clear();
	//imageAvailableSemaphores.clear();
}


QeDataViewport::~QeDataViewport() {}

QeDataRender::~QeDataRender() {

	size_t size = frameBuffers.size();

	for (size_t i = 0; i < size; i++) {
		vkDestroyFramebuffer(VK->device, frameBuffers[i], nullptr);
	}
	frameBuffers.clear();

	vkFreeCommandBuffers(VK->device, VK->commandPool, uint32_t(commandBuffers.size()), commandBuffers.data());
	commandBuffers.clear();

	if (renderPass) {
		vkDestroyRenderPass(VK->device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}
	if (semaphore) {
		vkDestroySemaphore(VK->device, semaphore, nullptr);
		semaphore = VK_NULL_HANDLE;
	}
	graphicsPipeline = nullptr;
}


void QeGraphics::init(QeAssetXML* _property) {

	initProperty = _property;

	bUpdateDrawCommandBuffers = true;
	bRecreateRender = true;
	createRender(0);
}

void QeGraphics::updateViewport() {
	
	size_t size = renders.size();

	for (size_t i = 0; i<size ;++i) {

		QeDataRender * render = renders[i];
		int width = int(render->viewport.width);
		int height = int(render->viewport.height);
		size_t size1 = render->viewports.size();

		if (size1 == 0) {
			addNewViewport(i);
			++size1;
		}

		int xNum;
		int yNum;
		float eWidth;
		float eHeight;
		int time1;
		int time2;
		if (width > height) {
			time1 = width / height;
			time2 = int(round(sqrtf(float(size1) / time1)));

			xNum = int(ceil(float(size1) / time2));
			yNum = time2;
		}
		else
		{
			time1 = height / width;
			time2 = int(round(sqrtf(float(size1) / time1)));

			yNum = int(ceil(float(size1) / time2));
			xNum = time2;
		}
		eWidth = float(width) / xNum;
		eHeight = float(height) / yNum;

		for (int j = 0; j < size1; ++j) {

			QeDataViewport * viewport = render->viewports[j];

			viewport->viewport.minDepth = 0.0f;
			viewport->viewport.maxDepth = 1.0f;
			viewport->viewport.x = (i%xNum)*eWidth;
			viewport->viewport.y = (i / xNum)*eHeight;
			viewport->viewport.height = eHeight;

			if ((i + 1) != size1)
				viewport->viewport.width = eWidth;
			else
				viewport->viewport.width = width - viewport->viewport.x;

			viewport->scissor.extent.width = int(viewport->viewport.width);
			viewport->scissor.extent.height = int(viewport->viewport.height);
			viewport->scissor.offset.x = int(viewport->viewport.x);
			viewport->scissor.offset.y = int(viewport->viewport.y);
			viewport->camera->faspect = viewport->viewport.width / viewport->viewport.height;
			viewport->camera->bUpdate = true;
		}
	}
}

void QeGraphics::popViewport(size_t renderIndex) {

	size_t size = renders[renderIndex]->viewports.size();
	if (size == 1) return;

	delete renders[renderIndex]->viewports[size - 1];
	renders[renderIndex]->viewports.pop_back();

	setTargetCamera(0);
	bRecreateRender = true;
}

void QeGraphics::addNewViewport(size_t renderIndex) {

	size_t size = renders[renderIndex]->viewports.size();
	QeDataViewport* vp = new QeDataViewport();
	//viewports.resize(size+1);
	renders[renderIndex]->viewports.push_back(vp);

	QeDataDescriptorSetCommon data;
	
	if (ACT->light) {
		vp->lights.push_back(ACT->light);
		VK->createBuffer(vp->lightsBuffer, sizeof(vp->lights[0]->bufferData), nullptr);
		data.lightsBuffer = vp->lightsBuffer.buffer;
	}

	QeAssetXML* node = AST->getXMLNode(initProperty, 1, "cameras");
	if (!node)	node = AST->getXMLNode(3, AST->CONFIG, "default", "cameras");
	
	size = node->nexts.size();

	int oid = 0;
	for (int index = 0; index < size; ++index) {

		AST->getXMLiValue(&oid, node->nexts[index], 1, "oid");

		if (renders[renderIndex]->cameraOID == 0 || oid == renders[renderIndex]->cameraOID) {
			node = node->nexts[index];
			break;
		}
	}

	vp->camera = (QeCamera*)OBJMGR->getObject(oid+int(renders[renderIndex]->viewports.size()-1), node);

	VK->createBuffer(vp->environmentBuffer, sizeof(vp->environmentData), nullptr);
	data.environmentBuffer = vp->environmentBuffer.buffer;

	VK->createDescriptorSet(vp->commonDescriptorSet);
	VK->updateDescriptorSet(&data, vp->commonDescriptorSet);

	bRecreateRender = true;
}

void QeGraphics::setTargetCamera( int index ) {
	
	if (index < renders[0]->viewports.size() && index != currentTargetViewport ) {
		currentTargetViewport = index;
		getTargetCamera()->updateAxis();
	}
}

QeCamera* QeGraphics::getTargetCamera() {
	return renders[0]->viewports[currentTargetViewport]->camera;
}

void QeGraphics::updateBuffer() {

	size_t size = renders.size();
	for (size_t i = 0; i < size; ++i) {
		
		QeDataRender * render = renders[i];
		size_t size1 = render->viewports.size();

		for (size_t j = 0; j < size1; ++j) {

			QeDataViewport * viewport = render->viewports[j];
			viewport->environmentData.ambientColor = ACT->ambientColor;
			viewport->environmentData.camera = viewport->camera->bufferData;
			viewport->environmentData.param.x = float(viewport->lights.size());

			VK->setMemoryBuffer(viewport->environmentBuffer,
				sizeof(viewport->environmentData), &viewport->environmentData);

			size_t size2 = viewport->lights.size();
			std::vector<QeDataLight> data;
			data.resize(size2);
			for (size_t k = 0; k < size2; ++k) {
					data[k] = viewport->lights[k]->bufferData;
			}
			VK->setMemoryBuffer(viewport->lightsBuffer, sizeof(data[0])*size2, data.data());
		}
	}
}

void QeGraphics::updateCompute(float time) {
	if (bRecreateRender) {
		vkDeviceWaitIdle(VK->device);

		cleanupRender();

		if(!renderCompleteSemaphore) renderCompleteSemaphore = VK->createSyncObjectSemaphore();
		if (!swapchain.khr)			VK->createSwapchain(&swapchain);
		size_t size = swapchain.images.size();

		if (fences.empty()) {
			fences.resize(size);

			for (size_t i = 0; i < size; ++i) {
				fences[i] = VK->createSyncObjectFence();
			}
		}
		refreshRender();
		updateViewport();

		OBJMGR->recreatePipeline();		
		bRecreateRender = false;
	}
	VK->pushConstants[0] = time;
	bUpdateDrawCommandBuffers = true;
}

void QeGraphics::updateRender(float time) {

	if (bRecreateRender) return;
	updateBuffer();

	if (bUpdateDrawCommandBuffers) {
		updateDrawCommandBuffers();
		bUpdateDrawCommandBuffers = false;
	}
	drawFrame();
}

void QeGraphics::drawFrame() {

	static int currentFrame = 0;

	vkWaitForFences(VK->device, 1, &fences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(VK->device, 1, &fences[currentFrame]);

	int size = int(renders.size());
	--size;
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(VK->device, swapchain.khr, std::numeric_limits<uint64_t>::max(), renders[size]->semaphore, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		bRecreateRender = true;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)	LOG("failed to acquire swap chain image! "+ result);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	for (int i = size; i>-1 ;--i ) {

		QeDataRender * render = renders[i];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &render->semaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.signalSemaphoreCount = 1;

		if (i == 0) {
			submitInfo.pCommandBuffers = &render->commandBuffers[imageIndex];
			submitInfo.pSignalSemaphores = &renderCompleteSemaphore;
		}
		else {
			submitInfo.pCommandBuffers = &render->commandBuffers[0];
			submitInfo.pSignalSemaphores = &renders[i - 1]->semaphore;
		}

		result = vkQueueSubmit(VK->graphicsQueue, 1, &submitInfo, fences[currentFrame]);
		if (result != VK_SUCCESS)	LOG("failed to submit draw command buffer! " + result);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderCompleteSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.khr;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(VK->presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)	bRecreateRender = true;
	else if (result != VK_SUCCESS)	LOG("failed to present swap chain image!");

	currentFrame = (++currentFrame) % swapchain.images.size();
	vkQueueWaitIdle(VK->graphicsQueue);
}

void QeGraphics::refreshRender() {
	size_t size = renders.size();
	if (!size) {
		createRender();
		size = 1;
	}

	for (size_t i = 0; i < size; ++i) {

		QeDataRender * render = renders[i];
		render->viewport.minDepth = 0.0f;
		render->viewport.maxDepth = 1.0f;
		render->viewport.x = 0.0f;
		render->viewport.y = 0.0f;

		if (i == 0){
			render->viewport.height = float(swapchain.extent.height);
			render->viewport.width = float(swapchain.extent.width);
		}
		else {
			render->viewport.height = 256.0f;
			render->viewport.width = 256.0f;
		}
		render->scissor.offset.x = int(render->viewport.x);
		render->scissor.offset.y = int(render->viewport.y);
		render->scissor.extent.height = int(render->viewport.height);
		render->scissor.extent.width = int(render->viewport.width);
		VK->createImage(render->depthImage, 0, render->scissor.extent, VK->findDepthFormat(), nullptr);

		if (i==0) {
			render->renderPass = VK->createRenderPass(swapchain.format, render->subpassNum, true);

			if (render->subpassNum > 1) {
				render->attachImage.type = eImage_inputAttach;
				VK->createImage(render->attachImage, 0, render->scissor.extent, VK_FORMAT_R8G8B8A8_UNORM, nullptr);
				render->graphicsPipeline = VK->createGraphicsPipeline(&render->graphicsShader, eGraphicsPipeLine_Postprogessing, render->renderPass);
				QeDataDescriptorSetPostprocessing data;
				data.inputAttachImageView = render->attachImage.view;
				VK->updateDescriptorSet(&data, render->descriptorSet);
			}
		}
		else {
			render->renderPass = VK->createRenderPass(VK_FORMAT_R8G8B8A8_UNORM, render->subpassNum, false);
			render->attachImage.type = eImage_render;
			VK->createImage(render->attachImage, 0, render->scissor.extent, VK_FORMAT_R8G8B8A8_UNORM, nullptr);
		}

		size_t size1 = render->frameBuffers.size();
		for (size_t j = 0; j < size1; ++j) {

			if (i == 0) {
				if (render->subpassNum > 1)
					render->frameBuffers[j] = VK->createFramebuffer(render->renderPass, render->scissor.extent, 
						3, render->attachImage.view, render->depthImage.view, swapchain.images[j].view );
				else
					render->frameBuffers[j] = VK->createFramebuffer(render->renderPass, render->scissor.extent, 
						2, swapchain.images[j].view, render->depthImage.view );
			}
			else {
				render->frameBuffers[j] = VK->createFramebuffer(render->renderPass, render->scissor.extent, 
					2,render->attachImage.view, render->depthImage.view );
			}
			render->commandBuffers[j] = VK->createCommandBuffer();
		}
	}
}

QeDataRender* QeGraphics::createRender(int cameraOID) {

	size_t size = renders.size();
	QeDataRender * render = new QeDataRender();
	renders.push_back(render);

	render->cameraOID = cameraOID;
	render->subpassNum = 1;

	size_t size1 = 1;
	if (size == 0 && cameraOID == 0) {

		QeAssetXML * node = AST->getXMLNode(initProperty, 1, "postprocessing");
		if (node) {
			render->subpassNum = 2;
			AST->setGraphicsShader(render->graphicsShader, AST->getXMLNode(initProperty, 1, "postprocessing"), "postprocessing");
			VK->createDescriptorSet(render->descriptorSet);
		}
		size1 = VK->getSwapchainSize();
	}

	render->frameBuffers.resize(size1);
	render->commandBuffers.resize(size1);
	render->commandBeginInfos.resize(size1);
	render->renderPassInfos.resize(size1);

	render->semaphore = VK->createSyncObjectSemaphore();
	addNewViewport(size);
	return render;
}


QeDataRender * QeGraphics::getSubRender(int cameraOID) {
	size_t size = renders.size();

	for (size_t i = 1; i<size; ++i) {
		if (renders[i]->viewports[0]->camera->oid == cameraOID) return renders[i];
	}
	return createRender(cameraOID);
}

void QeGraphics::cleanupRender() {

	//presentImage.~QeVKImage();
	//depthImage.~QeVKImage();

	//vkDestroyPipeline(VK->device, postprocessingPipeline, nullptr);
	//postprocessingPipeline = VK_NULL_HANDLE;
	size_t size, i;
	//size = renders.size();
	//for (i = 0;i<size;++i ) {
	//	renders[i]->~QeDataRender();
	//}
	//postprocessingPipeline = nullptr;

	//size = swapChainFramebuffers.size();
	size = swapchain.images.size();

	for ( i = 0; i < size; i++) {
	//	vkDestroyFramebuffer(VK->device, swapChainFramebuffers[i], nullptr);
	//	swapChainFramebuffers[i] = VK_NULL_HANDLE;
		//vkDestroyImageView(VK->device, swapChainImageViews[i], nullptr);
		//swapChainImageViews[i] = VK_NULL_HANDLE;
		//vkDestroyImageView(VK->device, swapChainImages[i].view, nullptr);
		swapchain.images[i].view = VK_NULL_HANDLE;
		swapchain.images[i].image = VK_NULL_HANDLE;
	}
	//swapChainImages.clear();

	//vkFreeCommandBuffers(VK->device, VK->commandPool, static_cast<uint32_t>(drawCommandBuffers.size()), drawCommandBuffers.data());
	//drawCommandBuffers.clear();
	
	//if (renderPass != VK_NULL_HANDLE) {
	//	vkDestroyRenderPass(VK->device, renderPass, nullptr);
	//		renderPass = VK_NULL_HANDLE;
	//}
	//swapChainImages.clear();

	if (swapchain.khr) {
		vkDestroySwapchainKHR(VK->device, swapchain.khr, nullptr);
		swapchain.khr = VK_NULL_HANDLE;
	}
	//swapChainImages.clear();

	for (size_t i = 0;i<VK->graphicsPipelines.size();++i ) {
		vkDestroyPipeline(VK->device, VK->graphicsPipelines[i]->graphicsPipeline, nullptr);
		delete VK->graphicsPipelines[i];
	}
	VK->graphicsPipelines.clear();

	if (OBJMGR)	OBJMGR->cleanupPipeline();
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

void QeGraphics::updateDrawCommandBuffers() {

	int size = int(renders.size());
	for (int i = 0; i<size ; ++i) {

		QeDataRender * render = renders[i];
		size_t size1 = render->commandBuffers.size();

		// graphics shader
		std::vector<VkClearValue> clearValues;
		clearValues.resize(render->subpassNum + 1);

		clearValues[0].color = { ACT->ambientColor.x+float(i)/2 , ACT->ambientColor.y, ACT->ambientColor.z, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		if (render->subpassNum > 1)
			clearValues[2].color = { ACT->ambientColor.x, ACT->ambientColor.y, ACT->ambientColor.z, 1.0f };

		for (size_t j = 0; j < size1; ++j) {
			//VkCommandBufferBeginInfo beginInfo = {};
			render->commandBeginInfos[j].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//render->commandBeginInfos[j].flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(render->commandBuffers[j], &render->commandBeginInfos[j]);

			// pushConstants
			VK->updatePushConstnats(render->commandBuffers[j]);

			//compute shader
			OBJMGR->updateComputeCommandBuffer(render->commandBuffers[j], render->viewports[0]->camera, &render->viewports[0]->commonDescriptorSet);
			
			//VkRenderPassBeginInfo renderPassInfo = {};
			render->renderPassInfos[j].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render->renderPassInfos[j].renderPass = render->renderPass;
			render->renderPassInfos[j].renderArea = render->scissor;
			render->renderPassInfos[j].clearValueCount = uint32_t(clearValues.size());
			render->renderPassInfos[j].pClearValues = clearValues.data();
			render->renderPassInfos[j].framebuffer = render->frameBuffers[j];

			vkCmdBeginRenderPass(render->commandBuffers[j], &render->renderPassInfos[j], VK_SUBPASS_CONTENTS_INLINE);

			size_t size2 = render->viewports.size();
			for (size_t k = 0; k < size2; ++k) {
				vkCmdSetViewport(render->commandBuffers[j], 0, 1, &render->viewports[k]->viewport);
				vkCmdSetScissor(render->commandBuffers[j], 0, 1, &render->viewports[k]->scissor);
				vkCmdSetLineWidth(render->commandBuffers[j], 2.0f);

				OBJMGR->updateDrawCommandBuffer(render->commandBuffers[j], render->viewports[k]->camera, &render->viewports[k]->commonDescriptorSet, render->renderPass);
			}

			if (render->subpassNum > 1) {
				vkCmdNextSubpass(render->commandBuffers[j], VK_SUBPASS_CONTENTS_INLINE);
				vkCmdSetViewport(render->commandBuffers[j], 0, 1, &render->viewport);
				vkCmdSetScissor(render->commandBuffers[j], 0, 1, &render->scissor);
				vkCmdBindDescriptorSets(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 2, 1, &render->descriptorSet.set, 0, nullptr);
				vkCmdBindPipeline(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, render->graphicsPipeline->graphicsPipeline);
				vkCmdDraw(render->commandBuffers[j], 1, 1, 0, 0);
			}
			vkCmdEndRenderPass(render->commandBuffers[j]);
			if (vkEndCommandBuffer(render->commandBuffers[j]) != VK_SUCCESS)	LOG("failed to record command buffer!");
		}
	}
}