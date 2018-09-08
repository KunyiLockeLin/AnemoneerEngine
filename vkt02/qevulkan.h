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

enum QeRenderType {
	eRender_KHR = 0,
	eRender_main = 1,
	eRender_color = 2,
	eRender_MAX = 3
};

enum QeVKBufferType {
	eBuffer = 0,
	eBuffer_vertex = 1,
	eBuffer_index = 2,
	eBuffer_storage_compute_shader_return = 3,
	eBuffer_vertex_texel = 4,
	eBuffer_uniform = 5,
	eBuffer_storage = 6,
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
	eImage_depthStencil = 0,
	eImage_swapchain = 1,
	eImage_attach = 2,
	eImage_2D = 3,
	eImage_cube = 4,
	eImage_render = 5,
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

enum QeDescriptorSetLayoutType {
	eDescriptorSetLayout_Model = 0,
	eDescriptorSetLayout_Common = 1,
	eDescriptorSetLayout_Postprocessing = 2,
};

struct QeDataDescriptorSet {
	QeDescriptorSetLayoutType type;
	VkDescriptorSet set= VK_NULL_HANDLE;
	bool bRender = false;

	QeDataDescriptorSet(QeDescriptorSetLayoutType _type) :type(_type) {}
	~QeDataDescriptorSet();
};

struct QeDataDescriptorSetLayout {
	VkDescriptorType type;
	int startID;
	int count;
};

struct QeDataDescriptorSetCommon {
	// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	VkBuffer environmentBuffer = VK_NULL_HANDLE;

	// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	VkBuffer lightsBuffer = VK_NULL_HANDLE;
};

struct QeDataDescriptorSetModel {
	// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	VkBuffer	modelBuffer = VK_NULL_HANDLE;
	//VkBuffer	modelViewportBuffer = VK_NULL_HANDLE;

	// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	//VkImageView diffuseMapImageViews = VK_NULL_HANDLE;
	//VkSampler	diffueMapSamplers = VK_NULL_HANDLE;
	VkImageView baseColorMapImageView = VK_NULL_HANDLE;
	VkSampler	baseColorMapSampler = VK_NULL_HANDLE;

	VkImageView cubeMapImageView = VK_NULL_HANDLE;
	VkSampler	cubeMapSampler = VK_NULL_HANDLE;

	VkImageView normalMapImageView = VK_NULL_HANDLE;
	VkSampler	normalMapSampler = VK_NULL_HANDLE;

	// VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
	VkBufferView texelBufferView = VK_NULL_HANDLE;

	// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
	VkBuffer	computeShaderoutputBuffer = VK_NULL_HANDLE;
};

struct QeDataDescriptorSetPostprocessing {

	// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	VkBuffer	buffer = VK_NULL_HANDLE;

	// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER/VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
	VkImageView inputAttachImageView = VK_NULL_HANDLE;
	VkSampler	inputAttachSampler = VK_NULL_HANDLE;
};

/* 10,000, - 32,767
eid: 10,000 - 30,000
line:		10,000 - 10,999
billboard:	11,000 - 11,999
cubemap:	12,000 - 12,999
particle:	13,000 - 13,999
model:		14,000 - 14,999

scene:		28,000 - 28,999


oid:  10,000 - 32,767
line:		10,000 - 10,999
billboard:	11,000 - 11,999
cubemap:	12,000 - 12,999
particle:	13,000 - 13,999
model:		14,000 - 14,999

point:		25,000 - 25,999
camera:		26,000 - 26,999
light:		27,000 - 27,999

*/

enum QeObjectType {

	eObject_Point = 25,
	eObject_Camera = 26,
	eObject_Light = 27,
	eObject_Scene = 28,

	eObject_Line = 10,
	eObject_Billboard = 11,
	eObject_Cubemap = 12,
	eObject_Particle = 13,
	eObject_Model = 14,
	eObject_Render = 15,
};

enum QeGraphicsPipelineOtherType {
	eGraphicsPipeLine_none = 0,
	eGraphicsPipeLine_normal = 1,
	eGraphicsPipeLine_stencilBuffer = 2,
};

struct QeDataGraphicsPipeline {
	QeObjectType objectType;
	QeGraphicsPipelineOtherType minorType;
	VkPipeline pipeline;
	QeAssetGraphicsShader* shader;
	VkRenderPass		renderPass;
	VkSampleCountFlagBits sampleCount;

	bool bAlpha;
	int subpass=0;
	//bool bStencilBuffer;
};

// VK_CHECK_RESULT
// VkDestroyer

class QeVulkan {

public:
	QeVulkan(QeGlobalKey& _key) {}
	~QeVulkan();

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

	std::vector<QeDataGraphicsPipeline*> graphicsPipelines;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;

	std::vector<std::vector<QeDataDescriptorSetLayout>> descriptorSetLayoutDatas = {
		{	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10, 3 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 20, 1 }, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 30, 1 }},

			{ { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1 }, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10, 1 } },

			{ { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER/*VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT*/, 10, 1 } }
	};

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

	VkDescriptorPool descriptorPool;
	VkPipelineLayout pipelineLayout;
	VkCommandPool commandPool;
	VkCommandBuffer tempCommandBuffer = VK_NULL_HANDLE;

	void createInstance();
	void setupDebugCallback();
	void pickPhysicalDevice();
	void createLogicalDevice();
	int getSwapchainSize();
	void createSwapchain(QeDataSwapchain* swapchain);
	VkSampleCountFlagBits getMaxUsableSampleCount();
	VkRenderPass createRenderPass(QeRenderType renderType, int subpassNum, std::vector<VkFormat>& formats);
	VkFramebuffer createFramebuffer( VkRenderPass renderPass, VkExtent2D size, std::vector<VkImageView>& views );
	VkCommandBuffer createCommandBuffer();
	VkSemaphore createSyncObjectSemaphore();
	VkFence createSyncObjectFence();
	VkSurfaceKHR createSurface(HWND& window, HINSTANCE& windowInstance);

	void createDescriptorSetLayout();
	VkPipelineLayout createPipelineLayout();
	void updatePushConstnats(VkCommandBuffer command_buffer);

	void createCommandPool();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthStencilFormat();
	//bool hasStencilComponent(VkFormat format);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer );
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
	void createDescriptorSet(QeDataDescriptorSet& descriptorSet);
	void updateDescriptorSet(void* data, QeDataDescriptorSet& descriptorSet);
	VkPipeline createGraphicsPipeline(QeDataGraphicsPipeline* data);
	VkPipeline createComputePipeline(VkShaderModule shader);
	VkShaderModule createShaderModel(void* data, VkDeviceSize size);
	//void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	
	void createBuffer(QeVKBuffer& buffer, VkDeviceSize size, void* data);
	void setMemoryBuffer(QeVKBuffer& buffer, VkDeviceSize size, void* data);
	void createImage(QeVKImage& image, VkDeviceSize dataSize, int imageCount, VkExtent2D& imageSize, VkFormat format, void* data, VkSampleCountFlagBits sampleCount= VK_SAMPLE_COUNT_1_BIT);
	void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int imageCount, uint32_t mipLevels = 1);
	void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, VkDeviceSize dataSize, int imageCount, VkExtent2D& imageSize);
};