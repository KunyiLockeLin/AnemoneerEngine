#pragma once

#include "qeheader.h"

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;
	int computeFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0 && computeFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

enum QeVKBufferType {
	eBuffer = 0,
	eBuffer_vertex = 1,
	eBuffer_index = 2,
	eBuffer_storage_compute_shader_return = 3,
	eBuffer_storage_texel = 4,
	eBuffer_uniform = 5,
};

struct QeVKBuffer {

	const QeVKBufferType type;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkBufferView view = VK_NULL_HANDLE;
	void* mapped = nullptr;

	QeVKBuffer(QeVKBufferType _type):type(_type) {}
	~QeVKBuffer();
};

enum QeVKImageType {
	eImage_depth = 0,
	eImage_swapchain = 1,
	eImage_inputAttach = 2,
	eImage_2D = 3,
	eImage_cube = 4,
};

struct QeVKImage {

	const QeVKImageType type;
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;
	//void* mapped = nullptr;

	QeVKImage(QeVKImageType _type):type(_type) {}
	~QeVKImage();
};

struct QeDataDescriptorSet {

	// descriptorSetgUBONumber
	VkBuffer	uboBuffer = VK_NULL_HANDLE;
	VkBuffer	lightBuffer = VK_NULL_HANDLE;
	VkBuffer	materialBuffer = VK_NULL_HANDLE;

	// descriptorSetgImageNumber
	VkImageView diffuseMapImageViews = VK_NULL_HANDLE;
	VkSampler	diffueMapSamplers = VK_NULL_HANDLE;
	VkImageView cubeMapImageViews = VK_NULL_HANDLE;
	VkSampler	cubeMapSamplers = VK_NULL_HANDLE;
	VkImageView normalMapImageViews = VK_NULL_HANDLE;
	VkSampler	normalMapSamplers = VK_NULL_HANDLE;

	// descriptorSetgInputAttachmentNumber
	VkImageView inputAttachImageViews = VK_NULL_HANDLE;

	// descriptorSetcStorageTexeLBufferNumber
	VkBufferView inputStorageTexeLBufferView = VK_NULL_HANDLE;
	VkBufferView outputStorageTexeLBufferView = VK_NULL_HANDLE;

	// descriptorSetcBufferNumber
	VkBuffer	outputBuffer = VK_NULL_HANDLE;
};

enum QeGraphicsPipelineType {
	eGraphicsPipeLine_Point = 0,
	eGraphicsPipeLine_Line = 1,
	eGraphicsPipeLine_Triangle = 2,
	eGraphicsPipeLine_Postprogessing = 3,
};

struct QeGraphicsPipelineStruct {
	VkPipeline graphicsPipeline;
	QeAssetShader* shader;
	QeGraphicsPipelineType type;
	bool bAlpha;
	uint8_t subpassIndex;
};

// VK_CHECK_RESULT
// VkDestroyer

class QeVulkan {

public:
	QeVulkan(QeGlobalKey& _key) {}
	~QeVulkan();

	const int MAX_FRAMES_IN_FLIGHT = 2;
	const std::vector<const char*> validationLayers = {	"VK_LAYER_LUNARG_standard_validation"};
	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	bool bInit = false;
	VkBool32 bShowMesh = false;
	VkBool32 bShowNormal = false;

	std::vector<float> pushConstants;

	void init();
	void updateRender(float time);
	void updateCompute(float time);

	VkDevice device;
	VkInstance instance;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProperties;

	std::vector<QeGraphicsPipelineStruct*> graphicsPipelines;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;

	const uint8_t descriptorSetgUBOStart = 0;
	const uint8_t descriptorSetgUBONumber = 3;
	const uint8_t descriptorSetgImageStart = 10;
	const uint8_t descriptorSetgImageNumber = 3;
	const uint8_t descriptorSetgInputAttachmentStart = 20;
	const uint8_t descriptorSetgInputAttachmentNumber = 1;
	const uint8_t descriptorSetcStorageTexeLBufferStart = 30;
	const uint8_t descriptorSetcStorageTexeLBufferNumber = 2;
	const uint8_t descriptorSetcBufferStart = 40;
	const uint8_t descriptorSetcBufferNumber = 1;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkPipelineLayout pipelineLayout;
	VkCommandPool commandPool;

	void createInstance();
	void setupDebugCallback();
	void pickPhysicalDevice();
	void createLogicalDevice();

	void createSwapChain(VkSurfaceKHR& surface, VkSwapchainKHR& swapChain, VkExtent2D& swapChainExtent, VkFormat& swapChainImageFormat, std::vector<QeVKImage>& swapChainImages);
	VkRenderPass createRenderPass(VkFormat& swapChainImageFormat);
	void createFramebuffers(std::vector<VkFramebuffer>& framebuffers, QeVKImage& sceneImage, QeVKImage& depthImage, std::vector<QeVKImage>& swapChainImages, VkExtent2D& swapChainExtent, VkRenderPass& renderPass);
	void createPresentDepthImage(QeVKImage& presentImage, QeVKImage& depthImage, VkExtent2D& swapChainExtent);
	void createCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, size_t size);
	void createSyncObjects(std::vector<VkSemaphore>& imageAvailableSemaphores, std::vector<VkSemaphore>& renderFinishedSemaphores, std::vector<VkFence>& inFlightFences);
	VkSurfaceKHR createSurface(HWND& window, HINSTANCE& windowInstance);

	VkDescriptorSetLayout createDescriptorSetLayout();
	VkPipelineLayout createPipelineLayout( VkDescriptorSetLayout& descriptorSetLayout);
	void updatePushConstnats(VkCommandBuffer command_buffer);

	void createCommandPool();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	//VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, bool bCubemap=false, uint32_t mipLevels = 1);
	//void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, bool bCubemap=false, uint32_t mipLevels = 1);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels=1);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layer);
	//void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	//void createBufferView(VkBuffer buffer, VkFormat format, VkBufferView & buffer_view);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

	void createDescriptorPool();
	VkDescriptorSet createDescriptorSet(VkDescriptorSetLayout& descriptorSetLayout);
	QeGraphicsPipelineStruct* createGraphicsPipeline(QeAssetShader* shader, QeGraphicsPipelineType type, bool bAlpha = false, uint8_t subpassIndex = 0);
	VkPipeline createComputePipeline(VkShaderModule shader);
	//void setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size, void** mapped);
	//void setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size);
	void updateDescriptorSet(QeDataDescriptorSet& data, VkDescriptorSet& descriptorSet);
	VkShaderModule createShaderModel(void* data, VkDeviceSize size);
	//VkSampler createTextureSampler();
	
	//void createImageData(void* data, VkFormat format, VkDeviceSize imageSize, int width, int height, VkImage& image, VkDeviceMemory& imageMemory, void** mapped, int layer=0, bool bCubemap=false);
	void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	//void createBufferData(void* data, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, void** mapped);
	//void createUniformBuffer(VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createBuffer(QeVKBuffer& buffer, VkDeviceSize size, void* data);
	void setMemoryBuffer(QeVKBuffer& buffer, VkDeviceSize size, void* data);
	void createImage(QeVKImage& image, VkDeviceSize size, uint16_t width, uint16_t height, VkFormat format, void* data);
	void setMemoryImage(QeVKImage& image, VkDeviceSize size, uint16_t width, uint16_t height, VkFormat format, void* data, uint8_t layer);
};