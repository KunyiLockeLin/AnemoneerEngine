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
	VkBuffer buffer;
	VkDeviceMemory memory;

	~QeVKBuffer();
};

struct QeVKImageBuffer {

	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;

	~QeVKImageBuffer();
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

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	void init();
	void update(float time);
	void deviceWaitIdle();

	VkDevice device;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProperties;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	VkCommandPool commandPool;


	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	//VkSemaphore textOverlayComplete;
	std::vector<VkCommandBuffer> drawCommandBuffers;

	bool bUpdateDrawCommandBuffers;
	VkDescriptorPool descriptorPool;
	VkRenderPass renderPass;

	const int modelDescriptorSetBufferNumber = 3;
	const int modelDescriptorSetTextureNumber = 1;
	VkDescriptorSetLayout modelDescriptorSetLayout;
	VkPipelineLayout modelPipelineLayout;

	bool bPost = false, bInitPost = false;
	const int postDescriptorSetBufferNumber = 3;
	const int postDescriptorSetTextureNumber = 1;
	VkDescriptorSetLayout postDescriptorSetLayout;
	VkDescriptorSet postDescriptorSet;
	VkPipelineLayout postPipelineLayout;
	VkPipeline postPipeline;
	VkSampler postSampler;
	QeAssetShader* pPostVert = nullptr;
	QeAssetShader* pPostGeom = nullptr;
	QeAssetShader* pPostFrag = nullptr;

	void cleanupSwapChain();

	void recreateSwapChain();
	void createInstance();
	void setupDebugCallback();
	void pickPhysicalDevice();
	void createLogicalDevice();

	void createSwapChain();
	void createSwapChainImageViews();
	void createRenderPass();
	VkDescriptorSetLayout createDescriptorSetLayout( int bufNum, int texNum);
	VkPipelineLayout createPipelineLayout( VkDescriptorSetLayout& descriptorSetLayout);
	void createFramebuffers();
	void createCommandPool();
	void createDepthResources();
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
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
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
	VkPipeline createPipeline(VkShaderModule* vertShader, VkShaderModule* geomShader, VkShaderModule* fragShader, VkBool32 bAlpha = VK_TRUE, VkBool32 bDepthTest = VK_TRUE, VkBool32 bVetex = VK_TRUE);
	void setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size);
	void updateDescriptorSet(VkBuffer* buffers, int* buffersSize, int bufferNum, VkSampler* samplers, VkImageView* imageViews, int texNum, VkDescriptorSet& descriptorSet);
	VkShaderModule createShaderModel(void* data, VkDeviceSize size);
	VkSampler createTextureSampler();

	void createImageData(void* data, VkFormat format, VkDeviceSize imageSize, int width, int height, VkImage& image, VkDeviceMemory& imageMemory);
	void createBufferData(void* data, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createUniformBuffer(VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void initPostProcessing();
	void updatePostProcessing();
};