#pragma once

#include "header.h"

#ifndef NDEBUG
static inline const char *vk_result_string(VkResult err) {
    switch (err) {
#define STR(r) \
    case r:    \
        return #r
        STR(VK_SUCCESS);
        STR(VK_NOT_READY);
        STR(VK_TIMEOUT);
        STR(VK_EVENT_SET);
        STR(VK_EVENT_RESET);
        STR(VK_INCOMPLETE);
        STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR(VK_ERROR_INITIALIZATION_FAILED);
        STR(VK_ERROR_DEVICE_LOST);
        STR(VK_ERROR_MEMORY_MAP_FAILED);
        STR(VK_ERROR_LAYER_NOT_PRESENT);
        STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR(VK_ERROR_FEATURE_NOT_PRESENT);
        STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR(VK_ERROR_TOO_MANY_OBJECTS);
        STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR(VK_ERROR_FRAGMENTED_POOL);
        STR(VK_ERROR_UNKNOWN);
        STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR(VK_ERROR_FRAGMENTATION);
        STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        STR(VK_ERROR_SURFACE_LOST_KHR);
        STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(VK_SUBOPTIMAL_KHR);
        STR(VK_ERROR_OUT_OF_DATE_KHR);
        STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(VK_ERROR_VALIDATION_FAILED_EXT);
        STR(VK_ERROR_INVALID_SHADER_NV);
        STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR(VK_ERROR_NOT_PERMITTED_EXT);
        STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#undef STR
        default:
            return "UNKNOWN_RESULT";
    }
}

//std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << vk_result_string(resolved_err);
#define ASSERT_VK_SUCCESS(condition)         \
    const VkResult resolved_err = condition; \
    if (resolved_err != VK_SUCCESS) {                                                                                        \
        std::ostringstream oss;                                                                                \
        oss << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " \
            << vk_result_string(resolved_err) <<"("<< resolved_err<<")"; \
        STACK(oss.str());                                                                                      \
        std::terminate(); \
    }
#else
#define ASSERT_VK_SUCCESS(condition)
#endif

struct QueueFamilyIndices {
    int graphicsFamily = -1;
    int presentFamily = -1;
    int computeFamily = -1;

    bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0 && computeFamily >= 0; }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QeVKBuffer {
    QeVKBufferType type;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkBufferView view = VK_NULL_HANDLE;
    void *mapped = nullptr;

    QeVKBuffer(QeVKBufferType _type) : type(_type) {}
    ~QeVKBuffer();
};

struct QeVKImage {
    QeVKImageType type;
    VkImageLayout layout;
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    // void* mapped = nullptr;

    QeVKImage(QeVKImageType _type) : type(_type) {}
    ~QeVKImage();
};

struct QeDataDescriptorSet {
    QeDescriptorSetLayoutType type;
    VkDescriptorSet set = VK_NULL_HANDLE;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    QeDataDescriptorSet(QeDescriptorSetLayoutType _type) : type(_type) {}
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
    VkBuffer modelBuffer = VK_NULL_HANDLE;
    // VkBuffer	modelViewportBuffer = VK_NULL_HANDLE;

    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    // VkImageView diffuseMapImageViews = VK_NULL_HANDLE;
    // VkSampler	diffueMapSamplers = VK_NULL_HANDLE;
    VkImageView baseColorMapImageView = VK_NULL_HANDLE;
    VkSampler baseColorMapSampler = VK_NULL_HANDLE;

    VkImageView cubeMapImageView = VK_NULL_HANDLE;
    VkSampler cubeMapSampler = VK_NULL_HANDLE;

    VkImageView normalMapImageView = VK_NULL_HANDLE;
    VkSampler normalMapSampler = VK_NULL_HANDLE;

    VkImageView metallicRoughnessMapImageView = VK_NULL_HANDLE;
    VkSampler metallicRoughnessMapSampler = VK_NULL_HANDLE;
};

struct QeDataDescriptorSetCompute {
    // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    VkBufferView texelBufferView = VK_NULL_HANDLE;

    // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    VkBuffer buffer = VK_NULL_HANDLE;
};

struct QeDataDescriptorSetRaytracing {
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    // std::vector<VkBuffer>	modelVertexBuffers;
    VkBuffer modelDatasBuffer;

    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler imageSampler = VK_NULL_HANDLE;
};

struct QeDataDescriptorSetPostprocessing {
    // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    VkBuffer buffer = VK_NULL_HANDLE;

    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER/VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
    VkImageView inputAttachImageView = VK_NULL_HANDLE;
    VkSampler inputAttachSampler = VK_NULL_HANDLE;
};

struct QeDataGraphicsPipeline {
    AE_GAMEOBJECT_TYPE componentType;
    QeGraphicsPipelineOtherType minorType;
    VkPipeline pipeline;
    QeAssetGraphicsShader *shader;
    VkRenderPass renderPass;
    VkSampleCountFlagBits sampleCount;

    bool bAlpha;
    int subpass = 0;
    // bool bStencilBuffer;
};

struct QeDataComputePipeline {
    VkPipeline pipeline;
    VkShaderModule shader = VK_NULL_HANDLE;
};

// VK_CHECK_RESULT
// VkDestroyer

class QeVulkan {
   public:
    QeVulkan(AeGlobalKey &_key) : emptyImage2D(eImage_2D), emptyImageCube(eImage_cube) {}
    ~QeVulkan();

    std::vector<const char *> validationLayers = {};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    bool bInit = false;
    bool bShowMesh = false;
    bool bShowNormal = false;
    QeVKImage emptyImage2D;
    QeVKImage emptyImageCube;

    std::vector<float> pushConstants;

    void initialize();
    void update1() {}
    void update2() {}

    VkDevice device;
    VkInstance instance;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT callback;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties deviceProperties;

    std::vector<QeDataGraphicsPipeline *> graphicsPipelines;
    std::vector<QeDataComputePipeline *> computePipelines;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue computeQueue;

    std::vector<std::vector<QeDataDescriptorSetLayout>> descriptorSetLayoutDatas = {
        {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10, 4}},

        {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10, 1}},

        {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, 1},
         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER /*VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT*/
          ,
          10, 1}},

        {{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 0, 1}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10, 1}},

        {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0, 1}, {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10, 1}},
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
    void createSwapchain(QeDataSwapchain *swapchain);
    VkRenderPass createRenderPass(AE_RENDER_TYPE renderType, int subpassNum, std::vector<VkFormat> &formats);
    VkFramebuffer createFramebuffer(VkRenderPass renderPass, VkExtent2D size, std::vector<VkImageView> &views);
    VkCommandBuffer createCommandBuffer();
    VkSemaphore createSyncObjectSemaphore();
    VkFence createSyncObjectFence();
    VkSurfaceKHR createSurface(HWND &window, HINSTANCE &windowInstance);

    void createDescriptorSetLayout();
    VkPipelineLayout createPipelineLayout();
    void updatePushConstnats(VkCommandBuffer command_buffer);

    void createCommandPool();
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthStencilFormat();
    // bool hasStencilComponent(VkFormat format);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice &device, VkSurfaceKHR &surface);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice &device, VkSurfaceKHR &surface);
    std::vector<const char *> getRequiredExtensions();
    void checkValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                                                        uint64_t obj, size_t location, int32_t code, const char *layerPrefix,
                                                        const char *msg, void *userData);

    VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback);
    void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                       const VkAllocationCallbacks *pAllocator);

    void createDescriptorPool();
    void createDescriptorSet(QeDataDescriptorSet &descriptorSet);
    // void updateDescriptorSetRayTracing(QeDataDescriptorSetRaytracing &descriptor, QeDataDescriptorSet &descriptorSet);
    void updateDescriptorSet(void *data, QeDataDescriptorSet &descriptorSet);
    VkPipeline createGraphicsPipeline(QeDataGraphicsPipeline *data);
    VkPipeline createComputePipeline(QeDataComputePipeline *data);
    VkShaderModule createShaderModel(void *data, VkDeviceSize size);
    // void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight,
    // uint32_t mipLevels);

    void createBuffer(QeVKBuffer &buffer, VkDeviceSize size, void *data);
    void setMemoryBuffer(QeVKBuffer &buffer, VkDeviceSize size, void *data);
    void createImage(QeVKImage &image, VkDeviceSize dataSize, int imageCount, VkExtent2D &imageSize, VkFormat format, void *data,
                     VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
    void transitionImageLayout(VkCommandBuffer cmdBuf, QeVKImage &image, VkImageLayout newLayout, int imageCount,
                               uint32_t mipLevels = 1);
    void copyBufferToImage(VkBuffer buffer, VkImage image, VkDeviceSize dataSize, int imageCount, VkExtent2D &imageSize);
};
