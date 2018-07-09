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

	~QeDataViewport();
};

struct QeDataRender {

	std::vector<QeDataViewport*> viewports;
	int cameraOID;
	VkViewport viewport;
	VkRect2D scissor;

	QeVKImage colorImage, depthStencilImage, multiSampleColorImage, multiSampleDepthStencilImage;
	std::vector<VkFramebuffer> frameBuffers;

	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore semaphore = VK_NULL_HANDLE;
	
	VkRenderPass renderPass;

	int subpassNum;
	QeAssetGraphicsShader graphicsShader;
	QeDataDescriptorSet descriptorSet;
	QeDataGraphicsPipeline graphicsPipeline;

	QeDataRender() :colorImage(eImage_inputAttach), depthStencilImage(eImage_depthStencil),
		multiSampleColorImage(eImage_multiSampleColor), multiSampleDepthStencilImage(eImage_multiSampleDepthStencil),
		descriptorSet(eDescriptorSetLayout_Postprocessing) {}
	~QeDataRender();
};

struct QeDataSwapchain {
	VkSwapchainKHR khr = VK_NULL_HANDLE;
	VkExtent2D extent;
	VkFormat format;
	std::vector<QeVKImage> images;
};

struct QeDataDrawCommand {
	VkCommandBuffer commandBuffer;
	QeCamera* camera;
	QeDataDescriptorSet* commonDescriptorSet;
	VkRenderPass renderPass;
	QeRenderType type;
};

class QeGraphics
{
public:

	QeGraphics(QeGlobalKey& _key) {}
	~QeGraphics();

	QeAssetXML* initProperty;

	std::vector<QeDataRender*> renders; // 0:main render
	QeDataSwapchain swapchain;

	int currentTargetViewport = 0;

	VkSemaphore renderCompleteSemaphore = VK_NULL_HANDLE;
	std::vector<VkFence> fences;
	VkSampleCountFlagBits sampleCount;

	void init(QeAssetXML* _property);
	void addNewViewport(size_t renderIndex = 0);
	void popViewport(size_t renderIndex = 0);
	void updateViewport();
	void updateBuffer();
	void updateRender(float time);
	void updateCompute(float time);
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();
	QeDataRender * getRender(QeRenderType type, int cameraOID, VkExtent2D renderSize);
	//bool bUpdateComputeCommandBuffers = false;
	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;

	//std::vector<VkSemaphore> computeSemaphores;
	//std::vector<VkFence> computeFences;

	//VkSemaphore textOverlayComplete;

	QeDataRender* createRender(QeRenderType type, int cameraOID, VkExtent2D renderSize);
	void refreshRender();
	void cleanupRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	//void updateComputeCommandBuffers();
};