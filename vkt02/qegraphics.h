#pragma once

#include "qeheader.h"


struct QeDataEnvironment {
	QeVector4f ambientColor;
	QeDataCamera camera;
	QeVector4f param; // 0: light num
};

struct QeDataViewport {
	VkViewport viewport;
	VkRect2D scissor;
	QeCamera* camera;
	std::vector<QeLight*> lights;

	QeDataDescriptorSet commonDescriptorSet;
	QeDataEnvironment environmentData;
	QeVKBuffer environmentBuffer;
	QeVKBuffer lightsBuffer;

	QeDataViewport():environmentBuffer(eBuffer_uniform), lightsBuffer(eBuffer_storage),
		commonDescriptorSet(eDescriptorSetLayout_Common) {}
};

struct QeDataSubRender {

	std::vector<QeDataViewport*> viewports;
	VkRect2D size;

	VkDescriptorImageInfo descriptor;
	QeVKImage presentImage, depthImage;
	VkRenderPass renderPass;
	VkFramebuffer frameBuffer;
	VkCommandBuffer commandBuffer;
	VkSemaphore semaphore = VK_NULL_HANDLE;
	QeAssetGraphicsShader graphicsShader;
	int subpassNum;
	QeDataDescriptorSet postprocessingDescriptorSet;
	QeDataGraphicsPipeline* postprocessingPipeline = nullptr;

	QeDataSubRender() :presentImage(eImage_inputAttach), depthImage(eImage_depth), postprocessingDescriptorSet(eDescriptorSetLayout_Postprocessing) {}
};

class QeGraphics
{
public:

	QeGraphics(QeGlobalKey& _key):presentImage(eImage_inputAttach), depthImage(eImage_depth), postprocessingDescriptorSet(eDescriptorSetLayout_Postprocessing) {}
	~QeGraphics();

	QeAssetXML* initProperty;

	std::vector<QeDataSubRender> subRenders;

	// main render
	int currentCommandViewport = 0;
	int currentTargetViewport = 0;
	std::vector<QeDataViewport*> viewports;
	VkViewport mainViewport;
	VkRect2D mainScissor;
	int subpassNum;
	QeVKImage presentImage, depthImage;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCommandBuffers;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<QeVKImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	QeAssetGraphicsShader graphicsShader;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;

	QeDataDescriptorSet postprocessingDescriptorSet;
	QeDataGraphicsPipeline* postprocessingPipeline = nullptr;
	//

	void init(QeAssetXML* _property);
	void addNewViewport();
	void popViewport();
	void updateViewport();
	void updateBuffer();
	void updateRender(float time);
	void updateCompute(float time);
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();

	//bool bUpdateComputeCommandBuffers = false;
	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;

	//std::vector<VkSemaphore> computeSemaphores;
	//std::vector<VkFence> computeFences;

	//VkSemaphore textOverlayComplete;

	void createRender();
	void createSubRender();
	void cleanupRender();
	void recreateRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	//void updateComputeCommandBuffers();
};