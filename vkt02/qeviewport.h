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

	//bool bUpdateComputeCommandBuffers = false;
	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;

	QeVKImage sceneImage;
	QeVKImage depthImage;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	//std::vector<VkSemaphore> computeSemaphores;
	//std::vector<VkFence> computeFences;

	//VkSemaphore textOverlayComplete;
	std::vector<VkCommandBuffer> drawCommandBuffers;
	//std::vector<VkCommandBuffer> computeCommandBuffers;

	VkDescriptorSet postprocessingDescriptorSet;
	VkPipeline		postprocessingPipeline = VK_NULL_HANDLE;
	QeAssetShader	shader;

	void createRender();
	void cleanupRender();
	void recreateRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	//void updateComputeCommandBuffers();
	void initPostProcessing();
	void updatePostProcessing();
};