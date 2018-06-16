#pragma once

#include "qeheader.h"


struct QeDataSubRender {

	std::vector<VkViewport> viewports;
	std::vector<VkRect2D> scissors;
	std::vector<QeCamera*> cameras;

	VkDescriptorImageInfo descriptor;
	QeVKImage presentImage, depthImage;
	VkRenderPass renderPass;
	VkCommandBuffer commandBuffer;
	VkSemaphore semaphore = VK_NULL_HANDLE;
	QeAssetShader shader;

	QeDataDescriptorSet postprocessingDescriptorSet;
	QeDataGraphicsPipeline* postprocessingPipeline = nullptr;

	QeDataSubRender():presentImage(eImage_inputAttach), depthImage(eImage_depth), postprocessingDescriptorSet(eDescriptorSetLayout_Postprocessing){}
};

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

class QeViewport
{
public:

	QeViewport(QeGlobalKey& _key):presentImage(eImage_inputAttach), depthImage(eImage_depth), postprocessingDescriptorSet(eDescriptorSetLayout_Postprocessing) {}
	~QeViewport();

	QeAssetXML* initProperty;

	std::vector<QeDataSubRender> subRenders;

	// main render
	int currentCommandViewport = 0;
	int currentTargetViewport = 0;
	std::vector<QeDataViewport*> viewports;
	VkViewport mainViewport;
	VkRect2D mainScissor;

	QeVKImage presentImage, depthImage;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> drawCommandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<QeVKImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	QeAssetShader shader;

	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

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
	void cleanupRender();
	void recreateRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	//void updateComputeCommandBuffers();
};