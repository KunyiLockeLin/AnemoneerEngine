#pragma once

#include "qeheader.h"

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QeVKBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	~QeVKBuffer();
};

struct QeVKImageBuffer {

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;

	~QeVKImageBuffer();
};


struct QeDataDescriptorSet {

	// descriptorSetBufferNumber
	VkBuffer	uboBuffer = VK_NULL_HANDLE;
	uint64_t	uboSize = 0;
	VkBuffer	lightBuffer = VK_NULL_HANDLE;
	uint64_t	lightSize = 0;
	VkBuffer	materialBuffer = VK_NULL_HANDLE;
	uint64_t	materialSize = 0;

	// descriptorSetImageNumber
	VkImageView diffuseMapImageViews = VK_NULL_HANDLE;
	VkSampler	diffueMapSamplers = VK_NULL_HANDLE;

	// descriptorSetInputAttachmentNumber
	VkImageView inputAttachImageViews = VK_NULL_HANDLE;

	QeDataDescriptorSet();
};

class QeVulkan
{
public:
	QeVulkan(QeGlobalKey& _key) {}
	~QeVulkan();

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

//#ifdef NDEBUG
//	const bool enableValidationLayers = true;
//#else
//	const bool enableValidationLayers = true;
//#endif
	bool bInit = false;
	void init();
	void update(float time);
	void deviceWaitIdle();

	VkDevice device;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProperties;
	VkSurfaceKHR surface;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	QeVKImageBuffer depthImage;
	QeVKImageBuffer sceneImage;

	VkCommandPool commandPool;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	//VkSemaphore textOverlayComplete;
	std::vector<VkCommandBuffer> drawCommandBuffers;

	bool bUpdateDrawCommandBuffers = false;
	bool bRecreateRender = false;
	VkDescriptorPool descriptorPool;
	VkRenderPass renderPass;

	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;

	const uint8_t descriptorSetBufferNumber = 3;
	const uint8_t descriptorSetImageNumber = 1;
	const uint8_t descriptorSetInputAttachmentNumber = 1;

	VkDescriptorSet postprocessingDescriptorSet;
	VkPipeline		postprocessingPipeline  = VK_NULL_HANDLE;
	QeAssetShader*	pPostProcessingVert = nullptr;
	QeAssetShader*	pPostProcessingGeom = nullptr;
	QeAssetShader*	pPostProcessingFrag = nullptr;

	void createRender();
	void cleanupRender();
	void recreateRender();
	void createInstance();
	void setupDebugCallback();
	void pickPhysicalDevice();
	void createLogicalDevice();

	void createSwapChain();
	void createSwapChainImageViews();
	void createRenderPass();
	VkDescriptorSetLayout createDescriptorSetLayout();
	VkPipelineLayout createPipelineLayout( VkDescriptorSetLayout& descriptorSetLayout);
	void createFramebuffers();
	void createCommandPool();
	void createSceneDepthImage();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);
	void createDrawCommandBuffers();
	void updateDrawCommandBuffers();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createSemaphores();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

	void drawFrame();

	VkSurfaceKHR createSurface(HWND& window, HINSTANCE& windowInstance);
	void createDescriptorPool();
	VkDescriptorSet createDescriptorSet(VkDescriptorSetLayout& descriptorSetLayout);
	VkPipeline createPipeline(VkShaderModule* vertShader, VkShaderModule* geomShader, VkShaderModule* fragShader, VkBool32 bAlpha = VK_TRUE, VkBool32 bDepthTest = VK_TRUE, VkBool32 bVetex = VK_TRUE, uint8_t subpassIndex = 0);
	void setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size);
	void updateDescriptorSet(QeDataDescriptorSet& data, VkDescriptorSet& descriptorSet);
	VkShaderModule createShaderModel(void* data, VkDeviceSize size);
	VkSampler createTextureSampler();

	void createImageData(void* data, VkFormat format, VkDeviceSize imageSize, int width, int height, VkImage& image, VkDeviceMemory& imageMemory);
	void createBufferData(void* data, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createUniformBuffer(VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void initPostProcessing();
	void updatePostProcessing();
};