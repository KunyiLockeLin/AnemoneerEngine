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

struct QeVKBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	~QeVKBuffer();
};

struct QeVKImageBuffer {

	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

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
	VkImageView cubeMapImageViews = VK_NULL_HANDLE;
	VkSampler	cubeMapSamplers = VK_NULL_HANDLE;
	VkImageView normalMapImageViews = VK_NULL_HANDLE;
	VkSampler	normalMapSamplers = VK_NULL_HANDLE;

	// descriptorSetInputAttachmentNumber
	VkImageView inputAttachImageViews = VK_NULL_HANDLE;

	QeDataDescriptorSet();
};

enum QePipelineType {
	ePipeLine_Point = 0,
	ePipeLine_Line = 1,
	ePipeLine_Triangle = 2,
	ePipeLine_Postprogessing = 3,
};

class QeVulkan
{
public:
	QeVulkan(QeGlobalKey& _key) {}
	~QeVulkan();

	const int MAX_FRAMES_IN_FLIGHT = 2;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;

	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;

	const uint8_t descriptorSetBufferNumber = 3;
	const uint8_t descriptorSetImageNumber = 3;
	const uint8_t descriptorSetInputAttachmentNumber = 1;

	void createInstance();
	void setupDebugCallback();
	void pickPhysicalDevice();
	void createLogicalDevice();

	void createSwapChain(VkSurfaceKHR& surface, VkSwapchainKHR& swapChain, VkExtent2D& swapChainExtent, VkFormat& swapChainImageFormat, std::vector<VkImage>& swapChainImages, std::vector<VkImageView>& swapChainImageViews);
	VkRenderPass createRenderPass(VkFormat& swapChainImageFormat);
	void createFramebuffers(std::vector<VkFramebuffer>& framebuffers, QeVKImageBuffer& sceneImage, QeVKImageBuffer& depthImage, std::vector<VkImageView>& swapChainImageViews, VkExtent2D& swapChainExtent, VkRenderPass& renderPass);
	void createSceneDepthImage(QeVKImageBuffer& sceneImage, QeVKImageBuffer& depthImage, VkExtent2D& swapChainExtent);
	void createDrawCommandBuffers(std::vector<VkCommandBuffer>& drawCommandBuffers, size_t size);
	void createSyncObjects(std::vector<VkSemaphore>& imageAvailableSemaphores, std::vector<VkSemaphore>& renderFinishedSemaphores, std::vector<VkFence>& inFlightFences);
	VkSurfaceKHR createSurface(HWND& window, HINSTANCE& windowInstance);

	VkDescriptorSetLayout createDescriptorSetLayout();
	VkPipelineLayout createPipelineLayout( VkDescriptorSetLayout& descriptorSetLayout);
	void updatePushConstnats(VkCommandBuffer command_buffer);

	void createCommandPool();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, bool bCubemap=false, uint32_t mipLevels = 1);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, bool bCubemap=false, uint32_t mipLevels = 1);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels=1);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layer);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
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
	VkPipeline createGraphicsPipeline(QeAssetShader* shader, QePipelineType type, uint8_t subpassIndex = 0);
	VkPipeline createComputePipeline(VkShaderModule shader);
	void setMemory(VkDeviceMemory& memory, void* data, VkDeviceSize size);
	void updateDescriptorSet(QeDataDescriptorSet& data, VkDescriptorSet& descriptorSet);
	VkShaderModule createShaderModel(void* data, VkDeviceSize size);
	VkSampler createTextureSampler();
	
	void createImageData(void* data, VkFormat format, VkDeviceSize imageSize, int width, int height, VkImage& image, VkDeviceMemory& imageMemory, int layer=0, bool bCubemap=false);
	void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void createBufferData(void* data, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createUniformBuffer(VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

};

// Vulkan Cookbook https://github.com/PacktPublishing/Vulkan-Cookbook 
/*
bool									  Ready;
VkDestroyer(VkInstance)                   Instance;
VkPhysicalDevice                          PhysicalDevice;
VkDestroyer(VkDevice)                     LogicalDevice;
VkDestroyer(VkSurfaceKHR)                 PresentationSurface;
QueueParameters                           GraphicsQueue;
QueueParameters                           ComputeQueue;
QueueParameters                           PresentQueue;
SwapchainParameters                       Swapchain;
VkDestroyer(VkCommandPool)                CommandPool;
std::vector<VkDestroyer(VkImage)>         DepthImages;
std::vector<VkDestroyer(VkDeviceMemory)>  DepthImagesMemory;
std::vector<FrameResources>               FramesResources;
static uint32_t const                     FramesCount = 3;
static VkFormat const                     DepthFormat = VK_FORMAT_D16_UNORM;

bool InitializeVulkan( HINSTANCE HInstance, HWND HWnd, VkPhysicalDeviceFeatures * desired_device_features = nullptr,
VkImageUsageFlags swapchain_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
bool use_depth = true, VkImageUsageFlags depth_attachment_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
bool CreateSwapchain(VkImageUsageFlags swapchain_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
bool use_depth = true, VkImageUsageFlags depth_attachment_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
void Deinitialize();

namespace VulkanCookbook {
	// Common
	// VulkanDestroyer.h
	// VkDestroyer<> - wrapper for automatic object destruction

	struct VkInstanceWrapper {
		VkInstance Handle;
	};

	struct VkDeviceWrapper {
		VkDevice Handle;
	};

	struct VkSurfaceKHRWrapper {
		VkSurfaceKHR Handle;
	};

	// Deleter functions

	template<class VkType>
	void DestroyVulkanObject(VkType object);

	template<>
	inline void DestroyVulkanObject<VkInstanceWrapper>(VkInstanceWrapper object) {
		vkDestroyInstance(object.Handle, nullptr);
	}

	template<>
	inline void DestroyVulkanObject<VkDeviceWrapper>(VkDeviceWrapper object) {
		vkDestroyDevice(object.Handle, nullptr);
	}

	template<class VkParent, class VkChild>
	void DestroyVulkanObject(VkParent parent, VkChild object);

	template<>
	inline void DestroyVulkanObject<VkInstance, VkSurfaceKHRWrapper>(VkInstance instance, VkSurfaceKHRWrapper surface) {
		vkDestroySurfaceKHR(instance, surface.Handle, nullptr);
	}

#define VK_DESTROYER_SPECIALIZATION( VkChild, VkDeleter )                                                   \
	struct VkChild##Wrapper {                                                                               \
		VkChild Handle;                                                                                     \
	};                                                                                                      \
	template<>                                                                                              \
	inline void DestroyVulkanObject<VkDevice, VkChild##Wrapper>(VkDevice device, VkChild##Wrapper object) { \
		VkDeleter( device, object.Handle, nullptr );                                                        \
	}

	VK_DESTROYER_SPECIALIZATION(VkSemaphore, vkDestroySemaphore)
		// VK_DESTROYER_SPECIALIZATION( VkCommandBuffer, vkFreeCommandBuffers ) <- command buffers are freed along with the pool
		VK_DESTROYER_SPECIALIZATION(VkFence, vkDestroyFence)
		VK_DESTROYER_SPECIALIZATION(VkDeviceMemory, vkFreeMemory)
		VK_DESTROYER_SPECIALIZATION(VkBuffer, vkDestroyBuffer)
		VK_DESTROYER_SPECIALIZATION(VkImage, vkDestroyImage)
		VK_DESTROYER_SPECIALIZATION(VkEvent, vkDestroyEvent)
		VK_DESTROYER_SPECIALIZATION(VkQueryPool, vkDestroyQueryPool)
		VK_DESTROYER_SPECIALIZATION(VkBufferView, vkDestroyBufferView)
		VK_DESTROYER_SPECIALIZATION(VkImageView, vkDestroyImageView)
		VK_DESTROYER_SPECIALIZATION(VkShaderModule, vkDestroyShaderModule)
		VK_DESTROYER_SPECIALIZATION(VkPipelineCache, vkDestroyPipelineCache)
		VK_DESTROYER_SPECIALIZATION(VkPipelineLayout, vkDestroyPipelineLayout)
		VK_DESTROYER_SPECIALIZATION(VkRenderPass, vkDestroyRenderPass)
		VK_DESTROYER_SPECIALIZATION(VkPipeline, vkDestroyPipeline)
		VK_DESTROYER_SPECIALIZATION(VkDescriptorSetLayout, vkDestroyDescriptorSetLayout)
		VK_DESTROYER_SPECIALIZATION(VkSampler, vkDestroySampler)
		VK_DESTROYER_SPECIALIZATION(VkDescriptorPool, vkDestroyDescriptorPool)
		// VK_DESTROYER_SPECIALIZATION( VkDescriptorSet, vkFreeDescriptorSets ) <- descriptor sets are freed along with the pool
		VK_DESTROYER_SPECIALIZATION(VkFramebuffer, vkDestroyFramebuffer)
		VK_DESTROYER_SPECIALIZATION(VkCommandPool, vkDestroyCommandPool)
		VK_DESTROYER_SPECIALIZATION(VkSwapchainKHR, vkDestroySwapchainKHR)

		// Class definition

		template<class VkTypeWrapper>
	class VkDestroyer {
	public:
		VkDestroyer() :
			DestroyerFunction(nullptr) {
			Object.Handle = VK_NULL_HANDLE;
		}

		VkDestroyer(std::function<void(VkTypeWrapper)> destroyer_function) :
			DestroyerFunction(destroyer_function) {
			Object.Handle = VK_NULL_HANDLE;
		}

		VkDestroyer(VkTypeWrapper object, std::function<void(VkTypeWrapper)> destroyer_function) :
			DestroyerFunction(destroyer_function) {
			Object.Handle = object.Handle;
		}

		~VkDestroyer() {
			if (DestroyerFunction && Object.Handle) {
				DestroyerFunction(Object);
			}
		}

		VkDestroyer(VkDestroyer<VkTypeWrapper> && other) :
			DestroyerFunction(other.DestroyerFunction) {
			Object.Handle = other.Object.Handle;
			other.Object.Handle = VK_NULL_HANDLE;
			other.DestroyerFunction = nullptr;
		}

		VkDestroyer& operator=(VkDestroyer<VkTypeWrapper> && other) {
			if (this != &other) {
				VkTypeWrapper object = Object;
				std::function<void(VkTypeWrapper)> destroyer_function = DestroyerFunction;

				Object.Handle = other.Object.Handle;
				DestroyerFunction = other.DestroyerFunction;

				other.Object.Handle = object.Handle;
				other.DestroyerFunction = destroyer_function;
			}
			return *this;
		}

		decltype(VkTypeWrapper::Handle) & operator*() {
			return Object.Handle;
		}

		decltype(VkTypeWrapper::Handle) const & operator*() const {
			return Object.Handle;
		}

		bool operator!() const {
			return Object.Handle == VK_NULL_HANDLE;
		}

		operator bool() const {
			return Object.Handle != VK_NULL_HANDLE;
		}

		VkDestroyer(VkDestroyer<VkTypeWrapper> const &) = delete;
		VkDestroyer& operator=(VkDestroyer<VkTypeWrapper> const &) = delete;

	private:
		VkTypeWrapper Object;
		std::function<void(VkTypeWrapper)> DestroyerFunction;
	};

	// Helper macro

#define VkDestroyer( VkType ) VkDestroyer<VkType##Wrapper>

	// Helper functions

	inline void InitVkDestroyer(VkDestroyer<VkInstanceWrapper> & destroyer) {
		destroyer = VkDestroyer<VkInstanceWrapper>(std::bind(DestroyVulkanObject<VkInstanceWrapper>, std::placeholders::_1));
	}

	inline void InitVkDestroyer(VkDestroyer<VkDeviceWrapper> & destroyer) {
		destroyer = VkDestroyer<VkDeviceWrapper>(std::bind(DestroyVulkanObject<VkDeviceWrapper>, std::placeholders::_1));
	}

	template<class VkParent, class VkType>
	inline void InitVkDestroyer(VkParent const & parent, VkDestroyer<VkType> & destroyer) {
		destroyer = VkDestroyer<VkType>(std::bind(DestroyVulkanObject<VkParent, VkType>, parent, std::placeholders::_1));
	}

	template<class VkParent, class VkType>
	inline void InitVkDestroyer(VkDestroyer<VkParent> const & parent, VkDestroyer<VkType> & destroyer) {
		destroyer = VkDestroyer<VkType>(std::bind(DestroyVulkanObject<decltype(VkParent::Handle), VkType>, *parent, std::placeholders::_1));
	}

	// Common.h
	bool IsExtensionSupported(std::vector<VkExtensionProperties> const & available_extensions, char const * const extension);

	// 01 Instance and Devices
	bool CheckAvailableInstanceExtensions(std::vector<VkExtensionProperties> & available_extensions);
	bool CreateVulkanInstance(std::vector<char const *> const & desired_extensions, VkInstance & instance);
	bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> & available_devices);
	bool CheckAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties> & available_extensions);
	void GetFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures & device_features, VkPhysicalDeviceProperties & device_properties);
	bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties> & queue_families);
	bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t & queue_family_index);

	struct QueueInfo {
		uint32_t           FamilyIndex;
		std::vector<float> Priorities;
	};

	bool CreateLogicalDevice(VkPhysicalDevice physical_device, std::vector< QueueInfo > queue_infos, std::vector<char const *> const & desired_extensions, VkPhysicalDeviceFeatures * desired_features, VkDevice & logical_device);
	void GetDeviceQueue(VkDevice logical_device, uint32_t queue_family_index, uint32_t queue_index, VkQueue & queue);
	bool CreateLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance instance, VkDevice & logical_device, VkQueue & graphics_queue, VkQueue & compute_queue);
	void DestroyLogicalDevice(VkDevice & logical_device);
	void DestroyVulkanInstance(VkInstance & instance);

	// 02 Image Presentation
	bool CreateVulkanInstanceWithWsiExtensionsEnabled(std::vector<char const *> & desired_extensions, VkInstance & instance);
	bool CreatePresentationSurface(VkInstance instance, HINSTANCE HInstance, HWND HWnd, VkSurfaceKHR & presentation_surface);
	bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, uint32_t & queue_family_index);
	bool CreateLogicalDeviceWithWsiExtensionsEnabled(VkPhysicalDevice physical_device, std::vector< QueueInfo > queue_infos, std::vector<char const *> & desired_extensions, VkPhysicalDeviceFeatures  * desired_features, VkDevice & logical_device);
	bool SelectDesiredPresentationMode(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkPresentModeKHR desired_present_mode, VkPresentModeKHR & present_mode);
	bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkSurfaceCapabilitiesKHR & surface_capabilities);
	bool SelectNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, uint32_t & number_of_images);
	bool ChooseSizeOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkExtent2D & size_of_images);
	bool SelectDesiredUsageScenariosOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkImageUsageFlags desired_usages, VkImageUsageFlags & image_usage);
	bool SelectTransformationOfSwapchainImages(VkSurfaceCapabilitiesKHR const & surface_capabilities, VkSurfaceTransformFlagBitsKHR desired_transform, VkSurfaceTransformFlagBitsKHR & surface_transform);
	bool SelectFormatOfSwapchainImages(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkSurfaceFormatKHR desired_surface_format, VkFormat & image_format, VkColorSpaceKHR & image_color_space);
	bool CreateSwapchain(VkDevice logical_device, VkSurfaceKHR presentation_surface, uint32_t image_count, VkSurfaceFormatKHR surface_format,
		VkExtent2D image_size, VkImageUsageFlags image_usage, VkSurfaceTransformFlagBitsKHR surface_transform,
		VkPresentModeKHR present_mode, VkSwapchainKHR & old_swapchain, VkSwapchainKHR & swapchain);
	bool GetHandlesOfSwapchainImages(VkDevice logical_device, VkSwapchainKHR swapchain, std::vector<VkImage> & swapchain_images);
	bool CreateSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, VkDevice logical_device,
		VkImageUsageFlags swapchain_image_usage, VkExtent2D & image_size, VkFormat & image_format,
		VkSwapchainKHR & old_swapchain, VkSwapchainKHR & swapchain, std::vector<VkImage> & swapchain_images);
	bool AcquireSwapchainImage(VkDevice logical_device, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t & image_index);

	struct PresentInfo {
		VkSwapchainKHR  Swapchain;
		uint32_t        ImageIndex;
	};

	bool PresentImage(VkQueue queue, std::vector<VkSemaphore> rendering_semaphores, std::vector<PresentInfo> images_to_present);
	void DestroySwapchain(VkDevice logical_device, VkSwapchainKHR & swapchain);
	void DestroyPresentationSurface(VkInstance instance, VkSurfaceKHR & presentation_surface);

	// 03 Command Buffers and Synchronization
	bool CreateCommandPool(VkDevice logical_device, VkCommandPoolCreateFlags command_pool_flags, uint32_t queue_family, VkCommandPool & command_pool);
	bool AllocateCommandBuffers(VkDevice logical_device, VkCommandPool command_pool, VkCommandBufferLevel level,
		uint32_t count, std::vector<VkCommandBuffer> & command_buffers);
	bool BeginCommandBufferRecordingOperation(VkCommandBuffer command_buffer, VkCommandBufferUsageFlags usage,
		VkCommandBufferInheritanceInfo * secondary_command_buffer_info);
	bool EndCommandBufferRecordingOperation(VkCommandBuffer command_buffer);
	bool ResetCommandBuffer(VkCommandBuffer command_buffer, bool release_resources);
	bool ResetCommandPool(VkDevice logical_device, VkCommandPool command_pool, bool release_resources);
	bool CreateSemaphore(VkDevice logical_device, VkSemaphore & semaphore);
	bool CreateFence(VkDevice logical_device, bool signaled, VkFence & fence);
	bool WaitForFences(VkDevice logical_device, std::vector<VkFence> const & fences, VkBool32 wait_for_all, uint64_t timeout);
	bool ResetFences(VkDevice logical_device, std::vector<VkFence> const & fences);

	struct WaitSemaphoreInfo {
		VkSemaphore           Semaphore;
		VkPipelineStageFlags  WaitingStage;
	};

	bool SubmitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos, std::vector<VkCommandBuffer> command_buffers,
		std::vector<VkSemaphore> signal_semaphores, VkFence fence);
	bool SynchronizeTwoCommandBuffers(VkQueue first_queue, std::vector<WaitSemaphoreInfo> first_wait_semaphore_infos,
		std::vector<VkCommandBuffer> first_command_buffers, std::vector<WaitSemaphoreInfo> synchronizing_semaphores,
		VkQueue second_queue, std::vector<VkCommandBuffer> second_command_buffers,
		std::vector<VkSemaphore> second_signal_semaphores, VkFence second_fence);
	bool CheckIfProcessingOfSubmittedCommandBufferHasFinished(VkDevice logical_device, VkQueue queue, std::vector<WaitSemaphoreInfo> wait_semaphore_infos,
		std::vector<VkCommandBuffer> command_buffers, std::vector<VkSemaphore> signal_semaphores,
		VkFence fence, uint64_t timeout, VkResult & wait_status);
	bool WaitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue);
	bool WaitForAllSubmittedCommandsToBeFinished(VkDevice logical_device);
	void DestroyFence(VkDevice logical_device, VkFence & fence);
	void DestroySemaphore(VkDevice logical_device, VkSemaphore & semaphore);
	void FreeCommandBuffers(VkDevice logical_device, VkCommandPool command_pool, std::vector<VkCommandBuffer> & command_buffers);
	void DestroyCommandPool(VkDevice logical_device, VkCommandPool & command_pool);

	// 04 Resources and Memory
	bool CreateBuffer(VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer & buffer);
	bool AllocateAndBindMemoryObjectToBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkBuffer buffer,
		VkMemoryPropertyFlagBits memory_properties, VkDeviceMemory & memory_object);

	struct BufferTransition {
		VkBuffer        Buffer;
		VkAccessFlags   CurrentAccess;
		VkAccessFlags   NewAccess;
		uint32_t        CurrentQueueFamily;
		uint32_t        NewQueueFamily;
	};

	void SetBufferMemoryBarrier(VkCommandBuffer command_buffer, VkPipelineStageFlags generating_stages,
		VkPipelineStageFlags consuming_stages, std::vector<BufferTransition> buffer_transitions);
	bool CreateBufferView(VkDevice logical_device, VkBuffer buffer, VkFormat format, VkDeviceSize memory_offset, VkDeviceSize memory_range, VkBufferView & buffer_view);
	bool CreateImage(VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps, uint32_t num_layers,
		VkSampleCountFlagBits samples, VkImageUsageFlags usage_scenarios, bool cubemap, VkImage & image);
	bool AllocateAndBindMemoryObjectToImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImage image,
		VkMemoryPropertyFlagBits memory_properties, VkDeviceMemory & memory_object);

	struct ImageTransition {
		VkImage             Image;
		VkAccessFlags       CurrentAccess;
		VkAccessFlags       NewAccess;
		VkImageLayout       CurrentLayout;
		VkImageLayout       NewLayout;
		uint32_t            CurrentQueueFamily;
		uint32_t            NewQueueFamily;
		VkImageAspectFlags  Aspect;
	};

	void SetImageMemoryBarrier(VkCommandBuffer command_buffer, VkPipelineStageFlags generating_stages,
		VkPipelineStageFlags consuming_stages, std::vector<ImageTransition> image_transitions);
	bool CreateImageView(VkDevice logical_device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect, VkImageView & image_view);
	bool Create2DImageAndView(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkExtent2D size,
		uint32_t num_mipmaps, uint32_t num_layers, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
		VkImageAspectFlags aspect, VkImage & image, VkDeviceMemory & memory_object, VkImageView & image_view);
	bool CreateLayered2DImageWithCubemapView(VkPhysicalDevice physical_device, VkDevice logical_device, uint32_t size, uint32_t num_mipmaps, VkImageUsageFlags usage,
		VkImageAspectFlags aspect, VkImage & image, VkDeviceMemory & memory_object, VkImageView & image_view);
	bool MapUpdateAndUnmapHostVisibleMemory(VkDevice logical_device, VkDeviceMemory memory_object, VkDeviceSize offset,
		VkDeviceSize data_size, void * data, bool unmap, void ** pointer);
	void CopyDataBetweenBuffers(VkCommandBuffer command_buffer, VkBuffer source_buffer, VkBuffer destination_buffer, std::vector<VkBufferCopy> regions);
	void CopyDataFromBufferToImage(VkCommandBuffer command_buffer, VkBuffer source_buffer, VkImage destination_image,
		VkImageLayout image_layout, std::vector<VkBufferImageCopy> regions);
	void CopyDataFromImageToBuffer(VkCommandBuffer command_buffer, VkImage source_image, VkImageLayout image_layout,
		VkBuffer destination_buffer, std::vector<VkBufferImageCopy> regions);
	bool UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize data_size,
		void * data, VkBuffer destination_buffer, VkDeviceSize destination_offset,
		VkAccessFlags destination_buffer_current_access, VkAccessFlags destination_buffer_new_access,
		VkPipelineStageFlags destination_buffer_generating_stages,
		VkPipelineStageFlags destination_buffer_consuming_stages,
		VkQueue queue, VkCommandBuffer command_buffer, std::vector<VkSemaphore> signal_semaphores);
	bool UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize data_size,
		void * data, VkImage destination_image, VkImageSubresourceLayers destination_image_subresource,
		VkOffset3D destination_image_offset, VkExtent3D destination_image_size,
		VkImageLayout destination_image_current_layout, VkImageLayout destination_image_new_layout,
		VkAccessFlags destination_image_current_access, VkAccessFlags destination_image_new_access,
		VkImageAspectFlags destination_image_aspect, VkPipelineStageFlags destination_image_generating_stages,
		VkPipelineStageFlags destination_image_consuming_stages, VkQueue queue,
		VkCommandBuffer command_buffer, std::vector<VkSemaphore> signal_semaphores);
	void DestroyImageView(VkDevice logical_device, VkImageView & image_view);
	void DestroyImage(VkDevice logical_device, VkImage & image);
	void DestroyBufferView(VkDevice logical_device, VkBufferView & buffer_view);
	void FreeMemoryObject(VkDevice logical_device, VkDeviceMemory & memory_object);
	void DestroyBuffer(VkDevice logical_device, VkBuffer & buffer);

	// 05 Descriptor Sets
	bool CreateSampler(VkDevice logical_device, VkFilter mag_filter, VkFilter min_filter, VkSamplerMipmapMode mipmap_mode,
		VkSamplerAddressMode u_address_mode, VkSamplerAddressMode v_address_mode, VkSamplerAddressMode w_address_mode,
		float lod_bias, bool anisotropy_enable, float max_anisotropy, bool compare_enable, VkCompareOp compare_operator,
		float min_lod, float max_lod, VkBorderColor border_color, bool unnormalized_coords, VkSampler & sampler);
	bool CreateSampledImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t num_mipmaps,
		uint32_t num_layers, VkImageUsageFlags usage, bool cubemap, VkImageViewType view_type, VkImageAspectFlags aspect,
		bool linear_filtering, VkImage & sampled_image, VkDeviceMemory & memory_object, VkImageView & sampled_image_view);
	bool CreateCombinedImageSampler(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size,
		uint32_t num_mipmaps, uint32_t num_layers, VkImageUsageFlags usage, bool cubemap, VkImageViewType view_type,
		VkImageAspectFlags aspect, VkFilter mag_filter, VkFilter min_filter, VkSamplerMipmapMode mipmap_mode,
		VkSamplerAddressMode u_address_mode, VkSamplerAddressMode v_address_mode, VkSamplerAddressMode w_address_mode,
		float lod_bias, bool anisotropy_enable, float max_anisotropy, bool compare_enable, VkCompareOp compare_operator,
		float min_lod, float max_lod, VkBorderColor border_color, bool unnormalized_coords, VkSampler & sampler,
		VkImage & sampled_image, VkDeviceMemory & memory_object, VkImageView & sampled_image_view);
	bool CreateStorageImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size,
		uint32_t num_mipmaps, uint32_t num_layers, VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspect,
		bool atomic_operations, VkImage & storage_image, VkDeviceMemory & memory_object, VkImageView & storage_image_view);
	bool CreateUniformTexelBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage,
		VkBuffer & uniform_texel_buffer, VkDeviceMemory & memory_object, VkBufferView & uniform_texel_buffer_view);
	bool CreateStorageTexelBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage,
		bool atomic_operations, VkBuffer & storage_texel_buffer, VkDeviceMemory & memory_object, VkBufferView & storage_texel_buffer_view);
	bool CreateUniformBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize size,
		VkBufferUsageFlags usage, VkBuffer & uniform_buffer, VkDeviceMemory & memory_object);
	bool CreateStorageBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage,
		VkBuffer & storage_buffer, VkDeviceMemory & memory_object);
	bool CreateInputAttachment(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageType type, VkFormat format, VkExtent3D size,
		VkImageUsageFlags usage, VkImageViewType view_type, VkImageAspectFlags aspect, VkImage & input_attachment,
		VkDeviceMemory & memory_object, VkImageView & input_attachment_image_view);
	bool CreateDescriptorSetLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayoutBinding> const & bindings, VkDescriptorSetLayout & descriptor_set_layout);
	bool CreateDescriptorPool(VkDevice logical_device, bool free_individual_sets, uint32_t max_sets_count,
		std::vector<VkDescriptorPoolSize> const & descriptor_types, VkDescriptorPool & descriptor_pool);
	bool AllocateDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSetLayout> const & descriptor_set_layouts,
		std::vector<VkDescriptorSet> & descriptor_sets);

	struct ImageDescriptorInfo {
		VkDescriptorSet                     TargetDescriptorSet;
		uint32_t                            TargetDescriptorBinding;
		uint32_t                            TargetArrayElement;
		VkDescriptorType                    TargetDescriptorType;
		std::vector<VkDescriptorImageInfo>  ImageInfos;
	};

	struct BufferDescriptorInfo {
		VkDescriptorSet                     TargetDescriptorSet;
		uint32_t                            TargetDescriptorBinding;
		uint32_t                            TargetArrayElement;
		VkDescriptorType                    TargetDescriptorType;
		std::vector<VkDescriptorBufferInfo> BufferInfos;
	};

	struct TexelBufferDescriptorInfo {
		VkDescriptorSet                     TargetDescriptorSet;
		uint32_t                            TargetDescriptorBinding;
		uint32_t                            TargetArrayElement;
		VkDescriptorType                    TargetDescriptorType;
		std::vector<VkBufferView>           TexelBufferViews;
	};

	struct CopyDescriptorInfo {
		VkDescriptorSet     TargetDescriptorSet;
		uint32_t            TargetDescriptorBinding;
		uint32_t            TargetArrayElement;
		VkDescriptorSet     SourceDescriptorSet;
		uint32_t            SourceDescriptorBinding;
		uint32_t            SourceArrayElement;
		uint32_t            DescriptorCount;
	};

	void UpdateDescriptorSets(VkDevice logical_device, std::vector<ImageDescriptorInfo> const & image_descriptor_infos,
		std::vector<BufferDescriptorInfo> const & buffer_descriptor_infos, std::vector<TexelBufferDescriptorInfo> const & texel_buffer_descriptor_infos,
		std::vector<CopyDescriptorInfo> const & copy_descriptor_infos);
	void BindDescriptorSets(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_type, VkPipelineLayout pipeline_layout,
		uint32_t index_for_first_set, std::vector<VkDescriptorSet> const & descriptor_sets, std::vector<uint32_t> const & dynamic_offsets);
	bool CreateDescriptorsWithTextureAndUniformBuffer(VkPhysicalDevice physical_device, VkDevice logical_device, VkExtent3D sampled_image_size, uint32_t uniform_buffer_size,
		VkSampler & sampler, VkImage & sampled_image, VkDeviceMemory & sampled_image_memory_object, VkImageView & sampled_image_view,
		VkBuffer & uniform_buffer, VkDeviceMemory & uniform_buffer_memory_object, VkDescriptorSetLayout & descriptor_set_layout,
		VkDescriptorPool & descriptor_pool, std::vector<VkDescriptorSet> & descriptor_sets);
	bool FreeDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSet> & descriptor_sets);
	bool ResetDescriptorPool(VkDevice logical_device, VkDescriptorPool descriptor_pool);
	void DestroyDescriptorPool(VkDevice logical_device, VkDescriptorPool & descriptor_pool);
	void DestroyDescriptorSetLayout(VkDevice logical_device, VkDescriptorSetLayout & descriptor_set_layout);
	void DestroySampler(VkDevice logical_device, VkSampler & sampler);

	//06 Render Passes and Framebuffers
	void SpecifyAttachmentsDescriptions(std::vector<VkAttachmentDescription> const & attachments_descriptions);

	struct SubpassParameters {
		VkPipelineBindPoint                  PipelineType;
		std::vector<VkAttachmentReference>   InputAttachments;
		std::vector<VkAttachmentReference>   ColorAttachments;
		std::vector<VkAttachmentReference>   ResolveAttachments;
		VkAttachmentReference const        * DepthStencilAttachment;
		std::vector<uint32_t>                PreserveAttachments;
	};

	void SpecifySubpassDescriptions(std::vector<SubpassParameters> const & subpass_parameters, std::vector<VkSubpassDescription> & subpass_descriptions);
	void SpecifyDependenciesBetweenSubpasses(std::vector<VkSubpassDependency> const & subpasses_dependencies);
	bool CreateRenderPass(VkDevice logical_device, std::vector<VkAttachmentDescription> const & attachments_descriptions,
		std::vector<SubpassParameters> const & subpass_parameters, std::vector<VkSubpassDependency> const & subpass_dependencies, VkRenderPass & render_pass);
	bool CreateFramebuffer(VkDevice logical_device, VkRenderPass render_pass, std::vector<VkImageView> const & attachments,
		uint32_t width, uint32_t height, uint32_t layers, VkFramebuffer & framebuffer);
	bool PrepareRenderPassForGeometryRenderingAndPostprocessSubpasses(VkDevice logical_device, VkRenderPass render_pass);
	bool PrepareRenderPassAndFramebufferWithColorAndDepthAttachments(VkPhysicalDevice physical_device, VkDevice logical_device, uint32_t width, uint32_t height,
		VkImage & color_image, VkDeviceMemory & color_image_memory_object, VkImageView & color_image_view,
		VkImage & depth_image, VkDeviceMemory & depth_image_memory_object, VkImageView & depth_image_view,
		VkRenderPass & render_pass, VkFramebuffer & framebuffer);
	void BeginRenderPass(VkCommandBuffer command_buffer, VkRenderPass render_pass, VkFramebuffer framebuffer, VkRect2D render_area,
		std::vector<VkClearValue> const & clear_values, VkSubpassContents subpass_contents);
	void ProgressToTheNextSubpass(VkCommandBuffer command_buffer, VkSubpassContents subpass_contents);
	void EndRenderPass(VkCommandBuffer command_buffer);
	void DestroyFramebuffer(VkDevice logical_device, VkFramebuffer & framebuffer);
	void DestroyRenderPass(VkDevice logical_device, VkRenderPass & render_pass);

	// 08 Graphics and Compute Pipelines

	bool CreateShaderModule(VkDevice logical_device, std::vector<unsigned char> const & source_code, VkShaderModule & shader_module);

	struct ShaderStageParameters {
		VkShaderStageFlagBits        ShaderStage;
		VkShaderModule               ShaderModule;
		char const                 * EntryPointName;
		VkSpecializationInfo const * SpecializationInfo;
	};

	void SpecifyPipelineShaderStages(std::vector<ShaderStageParameters> const & shader_stage_params, std::vector<VkPipelineShaderStageCreateInfo> & shader_stage_create_infos);
	void SpecifyPipelineVertexInputState(std::vector<VkVertexInputBindingDescription> const & binding_descriptions,
		std::vector<VkVertexInputAttributeDescription> const & attribute_descriptions,
		VkPipelineVertexInputStateCreateInfo & vertex_input_state_create_info);
	void SpecifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitive_restart_enable, VkPipelineInputAssemblyStateCreateInfo & input_assembly_state_create_info);
	void SpecifyPipelineTessellationState(uint32_t patch_control_points_count, VkPipelineTessellationStateCreateInfo & tessellation_state_create_info);

	struct ViewportInfo {
		std::vector<VkViewport>   Viewports;
		std::vector<VkRect2D>     Scissors;
	};

	void SpecifyPipelineViewportAndScissorTestState(ViewportInfo const & viewport_infos, VkPipelineViewportStateCreateInfo & viewport_state_create_info);
	void SpecifyPipelineRasterizationState(bool depth_clamp_enable, bool rasterizer_discard_enable, VkPolygonMode polygon_mode, VkCullModeFlags culling_mode,
		VkFrontFace front_face, bool depth_bias_enable, float depth_bias_constant_factor, float depth_bias_clamp,
		float depth_bias_slope_factor, float line_width, VkPipelineRasterizationStateCreateInfo & rasterization_state_create_info);
	void SpecifyPipelineMultisampleState(VkSampleCountFlagBits sample_count, bool per_sample_shading_enable, float min_sample_shading, VkSampleMask const * sample_masks,
		bool alpha_to_coverage_enable, bool alpha_to_one_enable, VkPipelineMultisampleStateCreateInfo & multisample_state_create_info);
	void SpecifyPipelineDepthAndStencilState(bool depth_test_enable, bool depth_write_enable, VkCompareOp depth_compare_op, bool depth_bounds_test_enable,
		float min_depth_bounds, float max_depth_bounds, bool stencil_test_enable, VkStencilOpState front_stencil_test_parameters,
		VkStencilOpState back_stencil_test_parameters, VkPipelineDepthStencilStateCreateInfo & depth_and_stencil_state_create_info);
	void SpecifyPipelineBlendState(bool logic_op_enable, VkLogicOp logic_op, std::vector<VkPipelineColorBlendAttachmentState> const & attachment_blend_states,
		std::array<float, 4> const & blend_constants, VkPipelineColorBlendStateCreateInfo & blend_state_create_info);
	void SpecifyPipelineDynamicStates(std::vector<VkDynamicState> const & dynamic_states, VkPipelineDynamicStateCreateInfo & dynamic_state_creat_info);
	bool CreatePipelineLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayout> const & descriptor_set_layouts,
		std::vector<VkPushConstantRange> const & push_constant_ranges, VkPipelineLayout & pipeline_layout);
	void SpecifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additional_options, std::vector<VkPipelineShaderStageCreateInfo> const & shader_stage_create_infos,
		VkPipelineVertexInputStateCreateInfo const & vertex_input_state_create_info,
		VkPipelineInputAssemblyStateCreateInfo const & input_assembly_state_create_info,
		VkPipelineTessellationStateCreateInfo const * tessellation_state_create_info,
		VkPipelineViewportStateCreateInfo const * viewport_state_create_info,
		VkPipelineRasterizationStateCreateInfo const & rasterization_state_create_info,
		VkPipelineMultisampleStateCreateInfo const * multisample_state_create_info,
		VkPipelineDepthStencilStateCreateInfo const * depth_and_stencil_state_create_info,
		VkPipelineColorBlendStateCreateInfo const * blend_state_create_info,
		VkPipelineDynamicStateCreateInfo const * dynamic_state_creat_info,
		VkPipelineLayout pipeline_layout, VkRenderPass render_pass,
		uint32_t subpass, VkPipeline base_pipeline_handle, int32_t base_pipeline_index,
		VkGraphicsPipelineCreateInfo & graphics_pipeline_create_info);
	bool CreatePipelineCacheObject(VkDevice logical_device, std::vector<unsigned char> const & cache_data, VkPipelineCache & pipeline_cache);
	bool RetrieveDataFromPipelineCache(VkDevice logical_device, VkPipelineCache pipeline_cache, std::vector<unsigned char> & pipeline_cache_data);
	bool MergeMultiplePipelineCacheObjects(VkDevice logical_device, VkPipelineCache target_pipeline_cache, std::vector<VkPipelineCache> const & source_pipeline_caches);
	bool CreateGraphicsPipelines(VkDevice logical_device, std::vector<VkGraphicsPipelineCreateInfo> const & graphics_pipeline_create_infos,
		VkPipelineCache pipeline_cache, std::vector<VkPipeline> & graphics_pipelines);
	bool CreateComputePipeline(VkDevice logical_device, VkPipelineCreateFlags additional_options, VkPipelineShaderStageCreateInfo const & compute_shader_stage,
		VkPipelineLayout pipeline_layout, VkPipeline base_pipeline_handle, VkPipelineCache pipeline_cache, VkPipeline & compute_pipeline);
	void BindPipelineObject(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_type, VkPipeline pipeline);
	bool CreatePipelineLayoutWithCombinedImageSamplerBufferAndPushConstantRanges(VkDevice logical_device, std::vector<VkPushConstantRange> const & push_constant_ranges,
		VkDescriptorSetLayout & descriptor_set_layout, VkPipelineLayout & pipeline_layout);
	bool CreateGraphicsPipelineWithVertexAndFragmentShadersDepthTestEnabledAndWithDynamicViewportAndScissorTests(VkDevice logical_device, VkPipelineCreateFlags additional_options,
		std::string const & vertex_shader_filename, std::string const & fragment_shader_filename,
		std::vector<VkVertexInputBindingDescription> const & vertex_input_binding_descriptions,
		std::vector<VkVertexInputAttributeDescription> const & vertex_attribute_descriptions,
		VkPrimitiveTopology primitive_topology, bool primitive_restart_enable, VkPolygonMode polygon_mode,
		VkCullModeFlags culling_mode, VkFrontFace front_face, bool logic_op_enable,
		VkLogicOp logic_op, std::vector<VkPipelineColorBlendAttachmentState> const & attachment_blend_states,
		std::array<float, 4> const & blend_constants, VkPipelineLayout pipeline_layout,
		VkRenderPass render_pass, uint32_t  subpass, VkPipeline base_pipeline_handle,
		VkPipelineCache pipeline_cache, std::vector<VkPipeline> & graphics_pipeline);
	bool CreateMultipleGraphicsPipelinesOnMultipleThreads(VkDevice logical_device, std::string const & pipeline_cache_filename,
		std::vector<std::vector<VkGraphicsPipelineCreateInfo>> const & graphics_pipelines_create_infos,
		std::vector<std::vector<VkPipeline>> & graphics_pipelines);
	void DestroyPipeline(VkDevice logical_device, VkPipeline & pipeline);
	void DestroyPipelineCache(VkDevice logical_device, VkPipelineCache & pipeline_cache);
	void DestroyPipelineLayout(VkDevice logical_device, VkPipelineLayout & pipeline_layout);
	void DestroyShaderModule(VkDevice logical_device, VkShaderModule & shader_module);

	// 09 Command Recording and Drawing
	void ClearColorImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout,
		std::vector<VkImageSubresourceRange> const & image_subresource_ranges, VkClearColorValue & clear_color);
	void ClearDepthStencilImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout image_layout,
		std::vector<VkImageSubresourceRange> const & image_subresource_ranges, VkClearDepthStencilValue & clear_value);
	void ClearRenderPassAttachments(VkCommandBuffer command_buffer, std::vector<VkClearAttachment> const & attachments, std::vector<VkClearRect> const & rects);

	struct VertexBufferParameters {
		VkBuffer      Buffer;
		VkDeviceSize  MemoryOffset;
	};

	void BindVertexBuffers(VkCommandBuffer command_buffer, uint32_t first_binding, std::vector<VertexBufferParameters> const & buffers_parameters);
	void BindIndexBuffer(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize memory_offset, VkIndexType index_type);
	void ProvideDataToShadersThroughPushConstants(VkCommandBuffer command_buffer, VkPipelineLayout pipeline_layout,
		VkShaderStageFlags pipeline_stages, uint32_t offset, uint32_t size, void * data);
	void SetViewportStateDynamically(VkCommandBuffer command_buffer, uint32_t first_viewport, std::vector<VkViewport> const & viewports);
	void SetScissorStateDynamically(VkCommandBuffer command_buffer, uint32_t first_scissor, std::vector<VkRect2D> const & scissors);
	void SetLineWidthStateDynamically(VkCommandBuffer command_buffer, float line_width);
	void SetDepthBiasStateDynamically(VkCommandBuffer command_buffer, float constant_factor, float clamp_value, float slope_factor);
	void SetBlendConstantsStateDynamically(VkCommandBuffer command_buffer, std::array<float, 4> const & blend_constants);
	void DrawGeometry(VkCommandBuffer command_buffer, uint32_t vertex_count, uint32_t instance_count, 
					  uint32_t first_vertex, uint32_t first_instance);
	void DrawIndexedGeometry(VkCommandBuffer command_buffer, uint32_t index_count, uint32_t instance_count, 
							 uint32_t first_index, uint32_t vertex_offset,	uint32_t first_instance);
	void DispatchComputeWork(VkCommandBuffer command_buffer, uint32_t x_size, uint32_t y_size, uint32_t z_size);
	void ExecuteSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer primary_command_buffer, 
																 std::vector<VkCommandBuffer> const & secondary_command_buffers);
	bool RecordCommandBufferThatDrawsGeometryWithDynamicViewportAndScissorStates(VkCommandBuffer command_buffer, VkImage swapchain_image,
											uint32_t present_queue_family_index, uint32_t graphics_queue_family_index, VkRenderPass render_pass,
											VkFramebuffer framebuffer, VkExtent2D framebuffer_size, std::vector<VkClearValue> const & clear_values,
											VkPipeline graphics_pipeline, uint32_t first_vertex_buffer_binding, 
											std::vector<VertexBufferParameters> const & vertex_buffers_parameters, VkPipelineLayout pipeline_layout,
											std::vector<VkDescriptorSet> const & descriptor_sets, uint32_t index_for_first_descriptor_set,
											Mesh const & geometry, uint32_t instance_count, uint32_t first_instance);
	
	struct CommandBufferRecordingThreadParameters {
		VkCommandBuffer                         CommandBuffer;
		std::function<bool(VkCommandBuffer)>	RecordingFunction;
	};

	bool RecordCommandBuffersOnMultipleThreads(std::vector<CommandBufferRecordingThreadParameters> const & threads_parameters, VkQueue queue,
											   std::vector<WaitSemaphoreInfo> wait_semaphore_infos, 
											   std::vector<VkSemaphore> signal_semaphores, VkFence fence);
	bool PrepareSingleFrameOfAnimation(VkDevice logical_device,	VkQueue graphics_queue, VkQueue present_queue, VkSwapchainKHR swapchain,
									   VkExtent2D swapchain_size, std::vector<VkImageView> const & swapchain_image_views, 
									   VkImageView depth_attachment, std::vector<WaitSemaphoreInfo> const & wait_infos,
									   VkSemaphore image_acquired_semaphore, VkSemaphore ready_to_present_semaphore, VkFence finished_drawing_fence,
									   std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> record_command_buffer,
									   VkCommandBuffer command_buffer, VkRenderPass render_pass, VkDestroyer(VkFramebuffer) & framebuffer);
	
	struct FrameResources {
		VkCommandBuffer             CommandBuffer;
		VkDestroyer(VkSemaphore)    ImageAcquiredSemaphore;
		VkDestroyer(VkSemaphore)    ReadyToPresentSemaphore;
		VkDestroyer(VkFence)        DrawingFinishedFence;
		VkDestroyer(VkImageView)    DepthAttachment;
		VkDestroyer(VkFramebuffer)  Framebuffer;

		FrameResources(VkCommandBuffer & command_buffer, VkDestroyer(VkSemaphore) & image_acquired_semaphore,
					   VkDestroyer(VkSemaphore) & ready_to_present_semaphore, VkDestroyer(VkFence) & drawing_finished_fence,
					   VkDestroyer(VkImageView) & depth_attachment, VkDestroyer(VkFramebuffer) & framebuffer) :
					   CommandBuffer(command_buffer), ImageAcquiredSemaphore(std::move(image_acquired_semaphore)),
					   ReadyToPresentSemaphore(std::move(ready_to_present_semaphore)),
					   DrawingFinishedFence(std::move(drawing_finished_fence)),
					   DepthAttachment(std::move(depth_attachment)), Framebuffer(std::move(framebuffer)) {
		}

		FrameResources(FrameResources && other) {
			*this = std::move(other);
		}

		FrameResources& operator=(FrameResources && other) {
			if (this != &other) {
				VkCommandBuffer command_buffer = CommandBuffer;

				CommandBuffer = other.CommandBuffer;
				other.CommandBuffer = command_buffer;
				ImageAcquiredSemaphore = std::move(other.ImageAcquiredSemaphore);
				ReadyToPresentSemaphore = std::move(other.ReadyToPresentSemaphore);
				DrawingFinishedFence = std::move(other.DrawingFinishedFence);
				DepthAttachment = std::move(other.DepthAttachment);
				Framebuffer = std::move(other.Framebuffer);
			}
			return *this;
		}

		FrameResources(FrameResources const &) = delete;
		FrameResources& operator=(FrameResources const &) = delete;
	};

	bool IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logical_device, VkQueue graphics_queue, VkQueue present_queue,
																				 VkSwapchainKHR swapchain, VkExtent2D swapchain_size, 
																				 std::vector<VkImageView> const & swapchain_image_views,
																			     VkRenderPass render_pass, 
																				 std::vector<WaitSemaphoreInfo> const & wait_infos, 
																				 std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> record_command_buffer,
																				 std::vector<FrameResources> & frame_resources);

	// CookbookSampleFramework.h
	struct QueueParameters {
		VkQueue   Handle;
		uint32_t  FamilyIndex;
	};

	struct SwapchainParameters {
		VkDestroyer(VkSwapchainKHR)             Handle;
		VkFormat                                Format;
		VkExtent2D                              Size;
		std::vector<VkImage>                    Images;
		std::vector<VkDestroyer(VkImageView)>   ImageViews;
		std::vector<VkImageView>                ImageViewsRaw;
	};

} // namespace VulkanCookbook
*/