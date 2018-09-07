#include "qeheader.h"


QeGraphics ::~QeGraphics() {
	cleanupRender();

	size_t size = fences.size();
	for (size_t i = 0; i < size; i++) {
		vkWaitForFences(VK->device, 1, &fences[i], VK_TRUE, UINT64_MAX);
		//vkResetFences(VK->device, 1, &fences[i]);
		vkDestroyFence(VK->device, fences[i], nullptr);
	}
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

	size = viewports.size();
	for (size_t i = 0; i < size; i++) {
		delete viewports[i];
	}
	viewports.clear();

	size = subpass.size();
	for (size_t i = 0; i < size; i++) {
		delete subpass[i];
	}
	subpass.clear();
}


void QeGraphics::init(QeAssetXML* _property) {

	initProperty = _property;

	bUpdateDrawCommandBuffers = true;
	bRecreateRender = true;
	createRender(eRender_KHR, 0, { 0,0 });
	createRender(eRender_main, 0, {0,0});
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
			viewport->viewport.x = (j%xNum)*eWidth;
			viewport->viewport.y = (j / xNum)*eHeight;
			viewport->viewport.height = eHeight;

			if ((j + 1) != size1)
				viewport->viewport.width = eWidth;
			else
				viewport->viewport.width = width - viewport->viewport.x;

			viewport->scissor.extent.width = int(viewport->viewport.width);
			viewport->scissor.extent.height = int(viewport->viewport.height);
			viewport->scissor.offset.x = int(viewport->viewport.x);
			viewport->scissor.offset.y = int(viewport->viewport.y);

			if (viewport->camera) {
				viewport->camera->faspect = viewport->viewport.width / viewport->viewport.height;
				viewport->camera->bUpdate = true;
			}
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
	bRecreateRender = true;

	if (renderIndex == eRender_KHR) return;

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

		if (oid == renders[renderIndex]->cameraOID) {
			node = node->nexts[index];
			break;
		}
	}

	vp->camera = (QeCamera*)OBJMGR->getObject(oid+int(renders[renderIndex]->viewports.size()-1), node);

	VK->createBuffer(vp->environmentBuffer, sizeof(vp->environmentData), nullptr);
	data.environmentBuffer = vp->environmentBuffer.buffer;

	VK->createDescriptorSet(vp->commonDescriptorSet);
	VK->updateDescriptorSet(&data, vp->commonDescriptorSet);
}

void QeGraphics::setTargetCamera( int index ) {
	
	if (index < renders[eRender_main]->viewports.size() && index != currentTargetViewport ) {
		currentTargetViewport = index;
		getTargetCamera()->updateAxis();
	}
}

QeCamera* QeGraphics::getTargetCamera() {
	return renders[eRender_main]->viewports[currentTargetViewport]->camera;
}

void QeGraphics::updateBuffer() {

	size_t size = renders.size();
	for (size_t i = 1; i < size; ++i) {
		
		QeDataRender * render = renders[i];
		size_t size1 = render->viewports.size();

		for (size_t j = 0; j < size1; ++j) {

			QeDataViewport * viewport = render->viewports[j];
			viewport->environmentData.ambientColor = ACT->ambientColor;
			viewport->environmentData.camera = viewport->camera->bufferData;
			viewport->environmentData.param.x = float(viewport->lights.size());

			AST->getXMLfValue(&viewport->environmentData.param.y, AST->getXMLNode(1, AST->CONFIG), 1, "gamma");
			AST->getXMLfValue(&viewport->environmentData.param.z, AST->getXMLNode(1, AST->CONFIG), 1, "exposure");

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

		size1 = render->subpass.size();

		for (size_t j = 0; j < size1; ++j) {
			VK->setMemoryBuffer(render->subpass[j]->buffer, sizeof(render->subpass[j]->bufferData), &render->subpass[j]->bufferData);
		}
	}
}

void QeGraphics::updateCompute(float time) {
	if (bRecreateRender) {

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
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(VK->device, swapchain.khr, UINT64_MAX, renderCompleteSemaphore, fences[currentFrame], &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		bRecreateRender = true;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)	LOG("failed to acquire swap chain image! "+ result);

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	--size;

	for (int i = size; i>-1 ;--i ) {

		QeDataRender * render = renders[i];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &render->semaphore;

		if(i==size)
			submitInfo.pWaitSemaphores = &renderCompleteSemaphore;
		else
			submitInfo.pWaitSemaphores = &renders[i+1]->semaphore;

		if (i == eRender_KHR)
			submitInfo.pCommandBuffers = &render->commandBuffers[imageIndex];
		else 
			submitInfo.pCommandBuffers = &render->commandBuffers[0];

		result = vkQueueSubmit(VK->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		//result = vkQueueSubmit(VK->graphicsQueue, 1, &submitInfo, fences[currentFrame]);

		if (result != VK_SUCCESS)	LOG("failed to submit draw command buffer! " + result);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renders[0]->semaphore;
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
	int size = (int)renders.size();
	if (!size) {
		createRender(eRender_KHR, 0, { 0,0 });
		createRender(eRender_main, 0, { 0,0 });
		size = 2;
	}

	for (int i = size-1; i > -1; --i) {

		QeDataRender * render = renders[i];
		std::vector<VkFormat> formats;
		std::vector<VkImageView> views;

		VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;// VK_FORMAT_R16G16B16A16_SFLOAT VK_FORMAT_R32G32B32A32_SFLOAT;

		if (i == eRender_main || i == eRender_KHR)
			render->scissor.extent = swapchain.extent;

		size_t size1 = render->subpass.size();

		if (i == eRender_KHR) {

			views.push_back(swapchain.images[0].view);
			formats.push_back(swapchain.format);

			QeDataDescriptorSetPostprocessing data;
			data.buffer = render->subpass[0]->buffer.buffer;
			//render->subpass[0]->descriptorSet.bRender = true;
			data.inputAttachImageView = renders[eRender_main]->colorImage.view;
			data.inputAttachSampler = renders[eRender_main]->colorImage.sampler;

			VK->updateDescriptorSet(&data, render->subpass[0]->descriptorSet);
			render->subpass[0]->graphicsPipeline.sampleCount = VK_SAMPLE_COUNT_1_BIT;
		}
		else if (i == eRender_color || i== eRender_main) {

			if (sampleCount != VK_SAMPLE_COUNT_1_BIT) {
				render->depthStencilImage.type = eImage_multiSampleDepthStencil;
				VK->createImage(render->depthStencilImage, 0, 1, render->scissor.extent, VK->findDepthStencilFormat(), nullptr, sampleCount);
				formats.push_back(VK->findDepthStencilFormat());
				views.push_back(render->depthStencilImage.view);

				VK->createImage(render->multiSampleColorImage, 0, 1, render->scissor.extent, format, nullptr, sampleCount);
				formats.push_back(format);
				views.push_back(render->multiSampleColorImage.view);
			}
			else {
				render->depthStencilImage.type = eImage_depthStencil;
				VK->createImage(render->depthStencilImage, 0, 1, render->scissor.extent, VK->findDepthStencilFormat(), nullptr);
				formats.push_back(VK->findDepthStencilFormat());
				views.push_back(render->depthStencilImage.view);
			}

			render->colorImage.type = eImage_inputAttach;
			VK->createImage(render->colorImage, 0, 1, render->scissor.extent, format, nullptr);

			if (size1 == 0) {
				views.push_back(render->colorImage.view);
				formats.push_back(format);
			}
			else{
				VK->createImage(render->colorImage2, 0, 1, render->scissor.extent, format, nullptr);

				int k = size1 % 2;

				for (size_t j = 0; j < size1; ++j, ++k) {

					render->subpass[j]->graphicsPipeline.sampleCount = VK_SAMPLE_COUNT_1_BIT;

					QeDataDescriptorSetPostprocessing data;
					data.buffer = render->subpass[j]->buffer.buffer;

					QeVKImage *image = nullptr;
					if (k % 2 == 0 ) {
						image = &render->colorImage;
					}
					else {
						image = &render->colorImage2;
					}
					//render->subpass[j]->descriptorSet.bRender = true;
					data.inputAttachImageView = image->view;
					data.inputAttachSampler = image->sampler;

					VK->updateDescriptorSet(&data, render->subpass[j]->descriptorSet);
					
					views.push_back(image->view);
					formats.push_back(format);
				}

				views.push_back(render->colorImage.view);
				formats.push_back(format);
			}
		}

		render->renderPass = VK->createRenderPass(QeRenderType(i), int(size1), formats);

		for (size_t j = 0; j < size1; ++j) {
			render->subpass[j]->graphicsPipeline.renderPass = render->renderPass;
		}

		render->scissor.offset = { 0, 0 };
		render->viewport.minDepth = 0.0f;
		render->viewport.maxDepth = 1.0f;
		render->viewport.x = float(render->scissor.offset.x);
		render->viewport.y = float(render->scissor.offset.y);
		render->viewport.height = float(render->scissor.extent.height);
		render->viewport.width = float(render->scissor.extent.width);

		size_t size2 = render->frameBuffers.size();
		for (size_t j = 0; j < size2; ++j) {
			if (i == eRender_KHR) {
				views[0] = swapchain.images[j].view;
			}
			render->frameBuffers[j] = VK->createFramebuffer(render->renderPass, render->scissor.extent, views);
			render->commandBuffers[j] = VK->createCommandBuffer();
		}
	}
}

QeDataRender* QeGraphics::createRender(QeRenderType type, int cameraOID, VkExtent2D renderSize) {

	size_t size = renders.size();
	QeDataRender * render = new QeDataRender();
	renders.push_back(render);

	if (size != eRender_KHR) {
		if (cameraOID == 0) {
			AST->getXMLiValue(&cameraOID, initProperty, 1, "cameraOID");
			if (cameraOID == 0)	AST->getXMLiValue(&cameraOID, AST->getXMLNode(2, AST->CONFIG, "default"), 1, "cameraOID");
		}
	}
	render->cameraOID = cameraOID;
	render->scissor.extent = renderSize;

	size_t size1 = 1;
	if (size == eRender_main) {

		QeAssetXML * node = AST->getXMLNode(initProperty, 1, "postprocessing");
		if (node) {

			QeVector4f param1;
			AST->getXMLfValue(&param1.x, node, 1, "p0");
			AST->getXMLfValue(&param1.y, node, 1, "p1");
			AST->getXMLfValue(&param1.z, node, 1, "p2");
			AST->getXMLfValue(&param1.w, node, 1, "p3");

			int count = 1;
			int type = 0; // 1: bloom

			if (strcmp(AST->getXMLValue(node, 1, "frag"), "bloomf.spv") == 0) {

				if (param1.x > 1.f) count = int(param1.x);
				type = 1;
			}

			for (int i = 0;i<count; ++i) {
				QeDataSubpass* data = new QeDataSubpass();

				data->graphicsPipeline.bAlpha = false;
				data->graphicsPipeline.objectType = eObject_Scene;
				data->graphicsPipeline.minorType = eGraphicsPipeLine_none;
				data->graphicsPipeline.subpass = i + 1;

				AST->setGraphicsShader(data->graphicsShader, AST->getXMLNode(initProperty, 1, "postprocessing"), "postprocessing");
				data->graphicsPipeline.shader = &data->graphicsShader;
				VK->createDescriptorSet(data->descriptorSet);
				
				data->bufferData.param1 = param1;
				
				if (type==1) {
					if (param1.x < 2)	data->bufferData.param1.x = param1.x;
					else				data->bufferData.param1.x = float(i);
				}

				VK->createBuffer(data->buffer, sizeof(data->bufferData), &data->bufferData);
				render->subpass.push_back(data);
			}
		}
	}
	else if(size == eRender_KHR){
		size1 = VK->getSwapchainSize();
		QeDataSubpass* data = new QeDataSubpass();

		data->graphicsPipeline.bAlpha = false;
		data->graphicsPipeline.objectType = eObject_Scene;
		data->graphicsPipeline.minorType = eGraphicsPipeLine_none;
		data->graphicsPipeline.subpass = 0;

		AST->setGraphicsShader(data->graphicsShader, nullptr, "postprocessing");
		data->graphicsPipeline.shader = &data->graphicsShader;
		VK->createDescriptorSet(data->descriptorSet);
		VK->createBuffer(data->buffer, sizeof(data->bufferData), &data->bufferData);
		render->subpass.push_back(data);
	}

	render->frameBuffers.resize(size1);
	render->commandBuffers.resize(size1);

	render->semaphore = VK->createSyncObjectSemaphore();
	addNewViewport(size);
	return render;
}


QeDataRender * QeGraphics::getRender(QeRenderType type, int cameraOID, VkExtent2D renderSize) {
	size_t size = renders.size();

	for (size_t i = 1; i<size; ++i) {
		if (renders[i]->viewports[0]->camera->oid == cameraOID) return renders[i];
	}
	return createRender(type, cameraOID, renderSize);
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
		vkDestroyPipeline(VK->device, VK->graphicsPipelines[i]->pipeline, nullptr);
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

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	int size = int(renders.size());
	for (int i = 0; i<size ; ++i) {

		QeDataRender * render = renders[i];
		size_t size1 = render->commandBuffers.size();

		// graphics shader
		std::vector<VkClearValue> clearValues;

		if ( i==eRender_KHR ) {
			clearValues.resize(1);
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		}
		else {
			if (sampleCount == VK_SAMPLE_COUNT_1_BIT)
				clearValues.resize(render->subpass.size() + 2);
			else
				clearValues.resize(render->subpass.size() + 3);

			clearValues[0].depthStencil = { 1.0f, 0 };

			size_t size2 = clearValues.size();
			for (size_t k = 1; k < size2; ++k) {
				clearValues[k].color = { ACT->ambientColor.x + i-1, ACT->ambientColor.y, ACT->ambientColor.z, 1.0f };
			}
		}
		renderPassInfo.renderPass = render->renderPass;
		renderPassInfo.renderArea = render->scissor;
		renderPassInfo.clearValueCount = uint32_t(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		for (size_t j = 0; j < size1; ++j) {
		
			vkBeginCommandBuffer(render->commandBuffers[j], &beginInfo);

			renderPassInfo.framebuffer = render->frameBuffers[j];

			if (i != eRender_KHR) {
				// pushConstants
				VK->updatePushConstnats(render->commandBuffers[j]);

				//compute shader
				vkCmdBindDescriptorSets(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 1, 1, &render->viewports[0]->commonDescriptorSet.set, 0, nullptr);
				OBJMGR->updateComputeCommandBuffer(render->commandBuffers[j], render->viewports[0]->camera, &render->viewports[0]->commonDescriptorSet);
			}
			vkCmdBeginRenderPass(render->commandBuffers[j], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			if (i==	eRender_KHR) {
				vkCmdSetViewport(render->commandBuffers[j], 0, 1, &render->viewport);
				vkCmdSetScissor(render->commandBuffers[j], 0, 1, &render->scissor);
				vkCmdBindDescriptorSets(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 2, 1, &render->subpass[0]->descriptorSet.set, 0, nullptr);
				vkCmdBindPipeline(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&render->subpass[0]->graphicsPipeline));
				vkCmdDraw(render->commandBuffers[j], 1, 1, 0, 0);
			}
			else {				
				vkCmdSetLineWidth(render->commandBuffers[j], 2.0f);

				size_t size2 = render->viewports.size();
				for (size_t k = 0; k < size2; ++k) {
					vkCmdSetViewport(render->commandBuffers[j], 0, 1, &render->viewports[k]->viewport);
					vkCmdSetScissor(render->commandBuffers[j], 0, 1, &render->viewports[k]->scissor);

					QeDataDrawCommand command;
					command.camera = render->viewports[k]->camera;
					command.commandBuffer = render->commandBuffers[j];
					command.commonDescriptorSet = &render->viewports[k]->commonDescriptorSet;
					command.renderPass = render->renderPass;
					command.type = QeRenderType(i);

					vkCmdBindDescriptorSets(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 1, 1, &render->viewports[k]->commonDescriptorSet.set, 0, nullptr);

					OBJMGR->updateDrawCommandBuffer(&command);
				}

				size2 = render->subpass.size();
				for (size_t k = 0; k < size2; ++k) {
					vkCmdNextSubpass(render->commandBuffers[j], VK_SUBPASS_CONTENTS_INLINE);
					vkCmdSetViewport(render->commandBuffers[j], 0, 1, &render->viewport);
					vkCmdSetScissor(render->commandBuffers[j], 0, 1, &render->scissor);
					vkCmdBindDescriptorSets(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 2, 1, &render->subpass[k]->descriptorSet.set, 0, nullptr);

					vkCmdBindPipeline(render->commandBuffers[j], VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&render->subpass[k]->graphicsPipeline));
					vkCmdDraw(render->commandBuffers[j], 1, 1, 0, 0);
				}
			}
			vkCmdEndRenderPass(render->commandBuffers[j]);
			if (vkEndCommandBuffer(render->commandBuffers[j]) != VK_SUCCESS)	LOG("failed to record command buffer!");
		}
	}
}