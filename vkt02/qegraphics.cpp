#include "qeheader.h"


QeGraphics ::~QeGraphics() {
	clear();
}

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

void QeGraphics::clear() {

	cleanupRender();

	lights.clear();
	models.clear();
	alphaModels.clear();
	renders.clear();
	lightsBuffer.~QeVKBuffer();

	size_t size = fences.size();
	for (size_t i = 0; i < size; i++) {
		vkWaitForFences(VK->device, 1, &fences[i], VK_TRUE, UINT64_MAX);
		//vkResetFences(VK->device, 1, &fences[i]);
		vkDestroyFence(VK->device, fences[i], nullptr);
	}
	fences.clear();
}

void QeGraphics::initialize() {

	clear();
	renders.resize(eRender_MAX);

	bRecreateRender = true;
	createRender(eRender_KHR, 0, { 0,0 });
	//createRender(eRender_main, 0, {0,0});
}

void QeGraphics::updateViewport() {
	
	size_t size = renders.size();

	for (size_t i = 0; i<size ;++i) {

		QeDataRender * render = renders[i];
		if (!render) continue;

		int width = int(render->viewport.width);
		int height = int(render->viewport.height);
		size_t size1 = render->viewports.size();

		if (size1 == 0) {
			addNewViewport(QeRenderType(i));
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
			}
		}
	}
}

void QeGraphics::addLight(QeLight* light) {
	bUpdateLight = true;
	lights.push_back(light);
}

void QeGraphics::removeLight(QeLight* light) {
	bUpdateLight = true;
	eraseElementFromVector(lights, light);
}

void QeGraphics::popViewport(QeRenderType type) {

	if (!renders[type]) return;

	size_t size = renders[type]->viewports.size();
	if (size == 1) return;

	delete renders[type]->viewports[size - 1];
	renders[type]->viewports.pop_back();

	setTargetCamera(0);
	bRecreateRender = true;
}

void QeGraphics::addNewViewport(QeRenderType type) {

	if (!renders[type]) return;

	size_t size = renders[type]->viewports.size();
	QeDataViewport* vp = new QeDataViewport();
	//viewports.resize(size+1);
	renders[type]->viewports.push_back(vp);
	bRecreateRender = true;

	if (type == eRender_KHR) return;

	QeDataDescriptorSetCommon data;

	vp->camera = (QeCamera*)OBJMGR->findComponent(eComponent_camera, renders[type]->cameraOID);

	VK->createBuffer(vp->environmentBuffer, sizeof(vp->environmentData), nullptr);
	data.environmentBuffer = vp->environmentBuffer.buffer;

	VK->createDescriptorSet(vp->commonDescriptorSet);
	VK->updateDescriptorSet(&data, vp->commonDescriptorSet);
}

void QeGraphics::setTargetCamera( int index ) {
	
	if (index < renders[eRender_main]->viewports.size() && index != currentTargetViewport ) {
		currentTargetViewport = index;
		//getTargetCamera()->updateAxis();
	}
}

QeCamera* QeGraphics::getTargetCamera() {
	return renders[eRender_main]->viewports[currentTargetViewport]->camera;
}

void QeGraphics::updateBuffer() {

	if (bUpdateLight) {
		lightsBuffer.~QeVKBuffer();
		
		if( lights.size()>0) {
			VK->createBuffer(lightsBuffer, sizeof(QeDataLight) * lights.size(), nullptr);
		}
	}

	if (lightsBuffer.buffer) {
		std::vector<QeDataLight> lightsData;
		std::vector<QeLight*>::iterator it = lights.begin();
		while (it != lights.end()) {
			lightsData.push_back((*it)->bufferData);
			++it;
		}
		VK->setMemoryBuffer(lightsBuffer, sizeof(QeDataLight) * lights.size(), lightsData.data());
	}

	QeAssetXML* node = AST->getXMLNode(3, AST->CONFIG, "setting", "render");
	AST->getXMLbValue(&VK->bShowMesh, node, 1, "mesh");
	AST->getXMLbValue(&VK->bShowNormal, node, 1, "normal");
	AST->getXMLfValue(&clearColor.x, node, 1, "clearColorR");
	AST->getXMLfValue(&clearColor.y, node, 1, "clearColorG");
	AST->getXMLfValue(&clearColor.z, node, 1, "clearColorB");

	size_t size = renders.size();
	for (size_t i = 1; i < size; ++i) {
		
		QeDataRender * render = renders[i];
		if (!render) continue;

		size_t size1 = render->viewports.size();

		for (size_t j = 0; j < size1; ++j) {

			QeDataViewport * viewport = render->viewports[j];
			viewport->environmentData.ambientColor = clearColor;
			viewport->environmentData.camera = viewport->camera->bufferData;
			viewport->environmentData.param.x = float(lights.size());

			AST->getXMLfValue(&viewport->environmentData.param.y, node, 1, "gamma");
			AST->getXMLfValue(&viewport->environmentData.param.z, node, 1, "exposure");

			VK->setMemoryBuffer(viewport->environmentBuffer,
				sizeof(viewport->environmentData), &viewport->environmentData);

			if (bUpdateLight) {

				QeDataDescriptorSetCommon data;
				data.environmentBuffer = viewport->environmentBuffer.buffer;
				data.lightsBuffer = lightsBuffer.buffer;
				VK->updateDescriptorSet(&data, viewport->commonDescriptorSet);
			}
		}

		size1 = render->subpass.size();

		for (size_t j = 0; j < size1; ++j) {
			VK->setMemoryBuffer(render->subpass[j]->buffer, sizeof(render->subpass[j]->bufferData), &render->subpass[j]->bufferData);
		}
	}

	bUpdateLight = false;
}

void QeGraphics::update1() {
	if (bRecreateRender) {

		cleanupRender();

		sampleCount = VK->getMaxUsableSampleCount();

 		renderCompleteSemaphore = VK->createSyncObjectSemaphore();
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

		bRecreateRender = false;
	}
	VK->pushConstants[0] = QE->deltaTime;
}

void QeGraphics::update2() {

	if (bRecreateRender) return;
	updateBuffer();
	updateDrawCommandBuffers();
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
		if (!render) continue;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &render->semaphore;

		if(i==size || !renders[i + 1])
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

	for (int i = size-1; i > -1; --i) {

		QeDataRender * render = renders[i];
		if (!render) continue;

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
			data.inputAttachImageView = renders[eRender_main]->colorImage.view;
			data.inputAttachSampler = renders[eRender_main]->colorImage.sampler;

			VK->updateDescriptorSet(&data, render->subpass[0]->descriptorSet);
			render->subpass[0]->graphicsPipeline.sampleCount = VK_SAMPLE_COUNT_1_BIT;
		}
		else if (i == eRender_color || i== eRender_main) {

			if(i==eRender_main)	render->depthStencilImage.~QeVKImage();
			if(!render->depthStencilImage.view) VK->createImage(render->depthStencilImage, 0, 1, render->scissor.extent, VK->findDepthStencilFormat(), nullptr, sampleCount);
			formats.push_back(VK->findDepthStencilFormat());
			views.push_back(render->depthStencilImage.view);

			if (sampleCount != VK_SAMPLE_COUNT_1_BIT) {

				if (i == eRender_main) render->multiSampleColorImage.~QeVKImage();
				if (!render->multiSampleColorImage.view)	 VK->createImage(render->multiSampleColorImage, 0, 1, render->scissor.extent, format, nullptr, sampleCount);
				formats.push_back(format);
				views.push_back(render->multiSampleColorImage.view);
			}

			if (i == eRender_main) render->colorImage.~QeVKImage();
			if (!render->colorImage.view) VK->createImage(render->colorImage, 0, 1, render->scissor.extent, format, nullptr);

			if (size1 == 0) {
				views.push_back(render->colorImage.view);
				formats.push_back(format);
			}
			else{
				if (i == eRender_main) render->colorImage2.~QeVKImage();
				if (!render->colorImage2.view) VK->createImage(render->colorImage2, 0, 1, render->scissor.extent, format, nullptr);

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
					if(i == eRender_color) render->subpass[j]->descriptorSet.bRender = true;
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
		if(render->renderPass)	vkDestroyRenderPass(VK->device, render->renderPass, nullptr);
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

			if (render->frameBuffers[j]) vkDestroyFramebuffer(VK->device, render->frameBuffers[j], nullptr);
			render->frameBuffers[j] = VK->createFramebuffer(render->renderPass, render->scissor.extent, views);
			
			if (!render->commandBuffers[j]) render->commandBuffers[j] = VK->createCommandBuffer();
		}
	}
}

bool QeGraphics::addPostProcssing(QeRenderType renderType, int cameraOID, int postprocessingOID) {

	if (renders[renderType]->cameraOID != cameraOID) return false;
	QePostProcessing* postprocessing = (QePostProcessing*)OBJMGR->findComponent(eComponent_postprocessing, postprocessingOID);
	if (!postprocessing) return false;

	bRecreateRender = true;

	int count = 1;
	int type = 0; // 1: bloom

	if (strcmp(AST->getXMLValue(postprocessing->initProperty, 1, "frag"), "bloomf.spv") == 0) {
		if (postprocessing->param1.x > 1.f) count = int(postprocessing->param1.x);
		type = 1;
	}

	for (int i = 0; i < count; ++i) {
		QeDataSubpass* data = new QeDataSubpass();

		data->graphicsPipeline.bAlpha = false;
		data->graphicsPipeline.componentType = eComponent_postprocessing;
		data->graphicsPipeline.minorType = eGraphicsPipeLine_none;
		data->graphicsPipeline.subpass = i + 1;

		AST->setGraphicsShader(data->graphicsShader, postprocessing->initProperty, "postprocessing");
		data->graphicsPipeline.shader = &data->graphicsShader;
		VK->createDescriptorSet(data->descriptorSet);

		data->bufferData.param1 = postprocessing->param1;

		if (type == 1) {
			if (postprocessing->param1.x < 2)	data->bufferData.param1.x = postprocessing->param1.x;
			else				data->bufferData.param1.x = float(i);
		}

		VK->createBuffer(data->buffer, sizeof(data->bufferData), &data->bufferData);
		renders[renderType]->subpass.push_back(data);
	}
	return true;
}


QeDataRender* QeGraphics::createRender(QeRenderType type, int cameraOID, VkExtent2D renderSize) {

	QeDataRender * render = new QeDataRender();
	renders[type] = render;

	render->cameraOID = cameraOID;
	render->scissor.extent = renderSize;

	size_t size1 = 1;
	if(type == eRender_KHR){
		size1 = VK->getSwapchainSize();
		QeDataSubpass* data = new QeDataSubpass();

		data->graphicsPipeline.bAlpha = false;
		data->graphicsPipeline.componentType = eComponent_postprocessing;
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
	addNewViewport(type);
	return render;
}


QeDataRender * QeGraphics::getRender(QeRenderType type, int cameraOID, VkExtent2D renderSize) {
	size_t size = renders.size();

	for (size_t i = 1; i<size; ++i) {
		QeDataRender * render = renders[i];
		if (!render) continue;

		if (render->viewports[0]->camera->oid == cameraOID) return render;
	}
	return createRender(type, cameraOID, renderSize);
}

void QeGraphics::cleanupRender() {

	size_t size, i;

	size = swapchain.images.size();
	for ( i = 0; i < size; i++) {
		swapchain.images[i].view = VK_NULL_HANDLE;
		swapchain.images[i].image = VK_NULL_HANDLE;
	}
	swapchain.images.clear();

	if (swapchain.khr) {
		vkDestroySwapchainKHR(VK->device, swapchain.khr, nullptr);
		swapchain.khr = VK_NULL_HANDLE;
	}
	if (renderCompleteSemaphore) {
		vkDestroySemaphore(VK->device, renderCompleteSemaphore, nullptr);
		renderCompleteSemaphore = VK_NULL_HANDLE;
	}
	for (size_t i = 0;i<VK->graphicsPipelines.size();++i ) {
		vkDestroyPipeline(VK->device, VK->graphicsPipelines[i]->pipeline, nullptr);
		delete VK->graphicsPipelines[i];
	}
	VK->graphicsPipelines.clear();
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
	for (int i = size-1; i>-1 ; --i) {

		QeDataRender * render = renders[i];
		if (!render || !render->renderPass) continue;

		size_t size1 = render->commandBuffers.size();

		// graphics shader
		std::vector<VkClearValue> clearValues;

		if ( i==eRender_KHR ) {
			clearValues.resize(1);
			clearValues[0].color = { clearColor.x, clearColor.y, clearColor.z, 1.0f };
		}
		else {
			if (sampleCount == VK_SAMPLE_COUNT_1_BIT)
				clearValues.resize(render->subpass.size() + 2);
			else
				clearValues.resize(render->subpass.size() + 3);

			clearValues[0].depthStencil = { 1.0f, 0 };

			size_t size2 = clearValues.size();
			for (size_t k = 1; k < size2; ++k) {
				clearValues[k].color = { clearColor.x, clearColor.y, clearColor.z, 1.0f };
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
				updateComputeCommandBuffer(render->commandBuffers[j], render->viewports[0]->camera, &render->viewports[0]->commonDescriptorSet);
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
				float lineWidth = 1.f;
				AST->getXMLfValue(&lineWidth, AST->getXMLNode(2, AST->CONFIG, "setting"), 1, "lineWidth");
				vkCmdSetLineWidth(render->commandBuffers[j], lineWidth);

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
					updateDrawCommandBuffer(&command);
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


void QeGraphics::sortAlphaModels(QeCamera * camera) {
	
	if (!camera) return;

	size_t size = alphaModels.size();

	for (size_t i = 0; i<size ;++i) {
		for (size_t j = i + 1; j < size; ++j) {
			float dis1 = MATH->length(camera->owner->transform->worldPosition() - alphaModels[i]->owner->transform->worldPosition());
			float dis2 = MATH->length(camera->owner->transform->worldPosition() - alphaModels[j]->owner->transform->worldPosition());

		if (dis1 < dis2) std::swap(alphaModels[i], alphaModels[j]);
		}
	}
}

void QeGraphics::updateDrawCommandBuffer(QeDataDrawCommand* command) {

	std::vector<QeModel*>::iterator it = models.begin();
	while (it != models.end()) {
		(*it)->updateDrawCommandBuffer(command);
		++it;
	}

	sortAlphaModels(command->camera);
	it = alphaModels.begin();
	while (it != alphaModels.end()) {
		(*it)->updateDrawCommandBuffer(command);
		++it;
	}
}

void QeGraphics::updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {

	std::vector<QeModel*>::iterator it = models.begin();
	while (it != models.end()) {
		(*it)->updateComputeCommandBuffer(commandBuffer, camera, commonDescriptorSet);
		++it;
	}

	it = alphaModels.begin();
	while (it != alphaModels.end()) {
		(*it)->updateComputeCommandBuffer(commandBuffer, camera, commonDescriptorSet);
		++it;
	}
}