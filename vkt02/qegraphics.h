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

	QeVKImage presentImage, depthImage, attachImage;
	std::vector<VkFramebuffer> frameBuffers;

	std::vector<VkCommandBufferBeginInfo> commandBeginInfos;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore semaphore = VK_NULL_HANDLE;
	
	VkRenderPass renderPass;
	std::vector<VkRenderPassBeginInfo> renderPassInfos;

	int subpassNum;
	QeAssetGraphicsShader graphicsShader;
	QeDataDescriptorSet descriptorSet;
	QeDataGraphicsPipeline* graphicsPipeline = nullptr;

	QeDataRender() :presentImage(eImage_2D), attachImage(eImage_inputAttach), depthImage(eImage_depth), 
		descriptorSet(eDescriptorSetLayout_Postprocessing) {}
	~QeDataRender();
};

enum QeRenderType {
	eRender_main = 0,
	eRender_mirror =1,
	eRender_shadow = 2,
	eRender_others = 3,
};

struct QeDataSwapchain {
	VkSwapchainKHR khr = VK_NULL_HANDLE;
	VkExtent2D extent;
	VkFormat format;
	std::vector<QeVKImage> images;
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

	void init(QeAssetXML* _property);
	void addNewViewport(size_t renderIndex = 0);
	void popViewport(size_t renderIndex = 0);
	void updateViewport();
	void updateBuffer();
	void updateRender(float time);
	void updateCompute(float time);
	void setTargetCamera(int index);
	QeCamera* getTargetCamera();
	QeDataRender * getSubRender(int cameraOID);
	//bool bUpdateComputeCommandBuffers = false;
	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;

	//std::vector<VkSemaphore> computeSemaphores;
	//std::vector<VkFence> computeFences;

	//VkSemaphore textOverlayComplete;

	QeDataRender* createRender(int cameraOID=0);
	void refreshRender();
	void cleanupRender();
	void drawFrame();
	void updateDrawCommandBuffers();
	//void updateComputeCommandBuffers();
};