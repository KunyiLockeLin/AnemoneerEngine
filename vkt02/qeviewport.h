#pragma once

#include "qeheader.h"

class QeViewport
{
public:
	QeViewport(QeGlobalKey& _key) {}
	~QeViewport();

	int currentNum = 0;
	int targetCamera = 0;
	std::array<VkViewport, MAX_VIEWPORT_NUM> viewports;
	std::array<VkRect2D, MAX_VIEWPORT_NUM>	scissors;
	std::array<QeCamera*, MAX_VIEWPORT_NUM> cameras;
	VkPipelineViewportStateCreateInfo viewportState;

	void init(QeAssetXML* _property);
	void addNewViewport();
	void popViewport();
	void updateViewport();
	void updateRender(float time);
	void updateCompute(float time);
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();

	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkRenderPass renderPass;

	QeVKImageBuffer sceneImage;
	QeVKImageBuffer depthImage;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	//VkSemaphore textOverlayComplete;
	std::vector<VkCommandBuffer> drawCommandBuffers;

	VkDescriptorSet postprocessingDescriptorSet;
	VkPipeline		postprocessingPipeline = VK_NULL_HANDLE;
	QeAssetShader	shader;

	void createRender();
	void cleanupRender();
	void recreateRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	void initPostProcessing();
	void updatePostProcessing();
	void updateInput(QeInputData & inputData);
};