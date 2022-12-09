#include <algorithm>
#include "gpu_objects.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

#define GET_INSTANCE_PROC_ADDR(dev, entrypoint)                                                              \
    {                                                                                                        \
        fp##entrypoint = (PFN_vk##entrypoint)vkGetInstanceProcAddr(dev->get_VkInstance(), "vk" #entrypoint); \
        if (fp##entrypoint == nullptr) {                                                                     \
            LOG("vkGetInstanceProcAddr failed to find vk" #entrypoint);                                      \
        }                                                                                                    \
    }

PFN_vkGetDeviceProcAddr get_device_proc_addr = nullptr;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                                                                    \
    {                                                                                                                            \
        if (!get_device_proc_addr)                                                                                               \
            get_device_proc_addr = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(dev->get_VkInstance(), "vkGetDeviceProcAddr"); \
        fp##entrypoint = (PFN_vk##entrypoint)get_device_proc_addr(dev->get_VkDevice(), "vk" #entrypoint);                        \
        if (fp##entrypoint == nullptr) {                                                                                         \
            LOG("vkGetDeviceProcAddr failed to find vk" #entrypoint);                                                            \
        }                                                                                                                        \
    }

static inline const char* string_VkResult(VkResult err) {
    switch (err) {
        CASE_STR(VK_SUCCESS);
        CASE_STR(VK_NOT_READY);
        CASE_STR(VK_TIMEOUT);
        CASE_STR(VK_EVENT_SET);
        CASE_STR(VK_EVENT_RESET);
        CASE_STR(VK_INCOMPLETE);
        CASE_STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        CASE_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        CASE_STR(VK_ERROR_INITIALIZATION_FAILED);
        CASE_STR(VK_ERROR_DEVICE_LOST);
        CASE_STR(VK_ERROR_MEMORY_MAP_FAILED);
        CASE_STR(VK_ERROR_LAYER_NOT_PRESENT);
        CASE_STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        CASE_STR(VK_ERROR_FEATURE_NOT_PRESENT);
        CASE_STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        CASE_STR(VK_ERROR_TOO_MANY_OBJECTS);
        CASE_STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        CASE_STR(VK_ERROR_FRAGMENTED_POOL);
        CASE_STR(VK_ERROR_UNKNOWN);
        CASE_STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        CASE_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        CASE_STR(VK_ERROR_FRAGMENTATION);
        CASE_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        CASE_STR(VK_ERROR_SURFACE_LOST_KHR);
        CASE_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        CASE_STR(VK_SUBOPTIMAL_KHR);
        CASE_STR(VK_ERROR_OUT_OF_DATE_KHR);
        CASE_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        CASE_STR(VK_ERROR_VALIDATION_FAILED_EXT);
        CASE_STR(VK_ERROR_INVALID_SHADER_NV);
        CASE_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        CASE_STR(VK_ERROR_NOT_PERMITTED_EXT);
        CASE_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        default:
            return "UNKNOWN_VkResult";
    }
}

#ifndef NDEBUG
#define ASSERT_VK_SUCCESS(condition) ASSERT_RESULT(condition, VkResult, VK_SUCCESS)
#define ASSERT_VK_NULL_HANDLE(condition) ASSERT(condition, "VK_NULL_HANDLE");
#else
#define ASSERT_VK_SUCCESS(condition) condition
#define ASSERT_VK_NULL_HANDLE(condition) condition;
#endif

Device::Device(const GPUObjectKey& key)
    : IGPUObject(AE_GPU_DEVICE), instance_(VK_NULL_HANDLE), physical_device_(VK_NULL_HANDLE), device_(VK_NULL_HANDLE) {}

const VkInstance Device::get_VkInstance() { return instance_; }
const VkPhysicalDevice Device::get_VkPhysicalDevice() { return physical_device_; }
const VkDevice Device::get_VkDevice() { return device_; }

AeResult Device::initialize(const InitializeInfo& initialize_info) {
    device_info_ = initialize_info.device_info_;
    ASSERT_SUCCESS(initialize_queues());
    ASSERT_SUCCESS(initialize_rendering(initialize_info.rendering_info_));

    ASSERT_SUCCESS(create_instance());
    ASSERT_SUCCESS(rendering_->initialize_for_VkInstance());
    ASSERT_SUCCESS(pick_physical_device());
    ASSERT_SUCCESS(create_device());
    ASSERT_SUCCESS(queues_->set_queues());
    ASSERT_SUCCESS(rendering_->initialize_for_VkDevice());

    return AE_SUCCESS;
}

AeResult Device::initialize_queues() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(GPU_MGR.get<Device>(device));
    ASSERT_SUCCESS(GPU_MGR.get<Queues>(queues_));
    ASSERT_SUCCESS(queues_->initialize(device));
    return AE_SUCCESS;
}

AeResult Device::initialize_rendering(const RenderingInfo& rendering_info) {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(GPU_MGR.get<Device>(device));
    ASSERT_SUCCESS(GPU_MGR.get<Rendering>(rendering_));
    ASSERT_SUCCESS(rendering_->initialize(device, rendering_info));
    return AE_SUCCESS;
}

AeResult Device::create_instance() {
    // If instance doesn't support 1.1+, it will returen fail.
    ASSERT_VK_SUCCESS(vkEnumerateInstanceVersion(&instance_api_version_));
    LOG("VkInstance API version: " + VK_API_VERSION_VARIANT(instance_api_version_) + "." +
        VK_API_VERSION_MAJOR(instance_api_version_) + "." + VK_API_VERSION_MINOR(instance_api_version_) + "." +
        VK_API_VERSION_PATCH(instance_api_version_));

    VkInstanceCreateInfo ins_ci = {};
    VkDebugUtilsMessengerCreateInfoEXT debug_ci = {};
    bool debug_messenger = false;
    ins_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "";
    app_info.applicationVersion = 0;
    app_info.pEngineName = "";
    app_info.engineVersion = 0;
    app_info.apiVersion = instance_api_version_;

    ins_ci.pApplicationInfo = &app_info;

    std::vector<const char*> extensions;
    auto& layers = device_info_.layer_names_;

    if (!layers.empty()) {
        ASSERT_SUCCESS(check_support_instance_layers(layers));

        for (const auto& layer : layers) {
            if (std::strncmp(layer, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

                debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debug_ci.messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debug_ci.messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
                debug_ci.pfnUserCallback = debug_messenger_callback;

                ins_ci.pNext = &debug_ci;
                debug_messenger = true;
            }
        }
    }
    extensions.emplace_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif  // VK_USE_PLATFORM_WIN32_KHR

    if (!extensions.empty()) {
        ASSERT_SUCCESS(check_support_instance_extensions(extensions));
    }

    ins_ci.enabledLayerCount = static_cast<uint32_t>(layers.size());
    ins_ci.ppEnabledLayerNames = layers.data();
    ins_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    ins_ci.ppEnabledExtensionNames = extensions.data();

    ASSERT_VK_SUCCESS(vkCreateInstance(&ins_ci, nullptr, &instance_));

    if (debug_messenger) {
        ASSERT_SUCCESS(create_debug_messenger_callback());
    }
    return AE_SUCCESS;
}

AeResult Device::create_debug_messenger_callback() {
    VkDebugUtilsMessengerCreateInfoEXT debug_ci = {};
    debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
    debug_ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    debug_ci.pfnUserCallback = debug_messenger_callback;

    GET_INSTANCE_PROC_ADDR(this, CreateDebugUtilsMessengerEXT);
    ASSERT_VK_SUCCESS(fpCreateDebugUtilsMessengerEXT(instance_, &debug_ci, nullptr, &debug_messenger_));
    return AE_SUCCESS;
}

VkBool32 Device::debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (pCallbackData) {
        LOG("Vulkan: " + pCallbackData->pMessage);
    }
    return VK_FALSE;
}

AeResult Device::check_support_instance_layers(std::vector<const char*>& layers) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateInstanceLayerProperties(&count, nullptr));
    if (!count) {
        LOG("VkInstance doesn't support any layer. Erase all reuqired layers");
        layers.clear();
        return AE_SUCCESS;
    }

    std::vector<VkLayerProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateInstanceLayerProperties(&count, prpos.data()));

    auto it = layers.begin();
    while (it != layers.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (std::strncmp(prop.layerName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkInstance supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkInstance doesn't support " + *it + ". Erase it.");
            it = layers.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::check_support_instance_extensions(std::vector<const char*>& extensions) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    if (!count) {
        LOG("VkInstance doesn't support any extension. Erase all reuqired extensions.");
        extensions.clear();
        return AE_SUCCESS;
    }

    std::vector<VkExtensionProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateInstanceExtensionProperties(nullptr, &count, prpos.data()));

    auto it = extensions.begin();
    while (it != extensions.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.extensionName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkInstance supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkInstance doesn't support " + *it + ". Erase it from the required list.");
            it = extensions.erase(it);
        }
    }
    return AE_SUCCESS;
}

AeResult Device::pick_physical_device() {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(instance_, &count, nullptr));
    if (!count) {
        LOG("No physical devices exist.");
        return AE_ERROR_UNKNOWN;
    }

    std::vector<VkPhysicalDevice> phy(count);
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(instance_, &count, phy.data()));
    uint32_t gpu_index = (device_info_.gpu_index_ < count) ? device_info_.gpu_index_ : (count - 1);
    physical_device_ = phy[gpu_index];

    phy_dev_props2_ = {};
    phy_dev_props2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(physical_device_, &phy_dev_props2_);

    LOG("Physical device: " + phy_dev_props2_.properties.deviceName +
        ", driver: " + (phy_dev_props2_.properties.driverVersion >> 22) + "." +
        ((phy_dev_props2_.properties.driverVersion >> 12) & 0x3ff) + "." + (phy_dev_props2_.properties.driverVersion & 0xfff) +
        ", API Version: " + VK_API_VERSION_VARIANT(phy_dev_props2_.properties.apiVersion) + "." +
        VK_API_VERSION_MAJOR(phy_dev_props2_.properties.apiVersion) + "." +
        VK_API_VERSION_MINOR(phy_dev_props2_.properties.apiVersion) + "." +
        VK_API_VERSION_PATCH(phy_dev_props2_.properties.apiVersion));

    phy_dev_feats2_ = {};
    phy_dev_feats2_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(physical_device_, &phy_dev_feats2_);
    return AE_SUCCESS;
}

AeResult Device::create_device() {
    VkPhysicalDeviceDescriptorIndexingFeatures desc_indexing_features = {};
    desc_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    desc_indexing_features.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &desc_indexing_features;
    features2.features.samplerAnisotropy = VK_TRUE;
    features2.features.fillModeNonSolid = VK_TRUE;
    features2.features.multiViewport = VK_TRUE;
    features2.features.geometryShader = VK_TRUE;
    features2.features.tessellationShader = VK_TRUE;
    features2.features.sampleRateShading = VK_TRUE;
    features2.features.wideLines = VK_TRUE;
    features2.features.vertexPipelineStoresAndAtomics = VK_TRUE;
    features2.features.fragmentStoresAndAtomics = VK_TRUE;
    features2.features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
    features2.features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;

    std::vector<const char*> extensions;
    extensions.emplace_back(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    if (!extensions.empty()) {
        ASSERT_SUCCESS(check_support_device_extensions(extensions));
    }

    std::vector<VkDeviceQueueCreateInfo> queue_cis;
    ASSERT_SUCCESS(queues_->get_device_queue_create_infos(queue_cis));

    VkDeviceCreateInfo dev_ci = {};
    dev_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dev_ci.pNext = &features2;
    dev_ci.queueCreateInfoCount = static_cast<uint32_t>(queue_cis.size());
    dev_ci.pQueueCreateInfos = queue_cis.data();
    dev_ci.enabledLayerCount = 0;  // Device layers has been Deprecated. It shouldn't be used.
    dev_ci.ppEnabledLayerNames = nullptr;
    dev_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    dev_ci.ppEnabledExtensionNames = extensions.data();
    ASSERT_VK_SUCCESS(vkCreateDevice(physical_device_, &dev_ci, nullptr, &device_));

    return AE_SUCCESS;
}

AeResult Device::check_support_device_extensions(std::vector<const char*>& extensions) {
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &count, nullptr));
    if (!count) {
        LOG("VkDevice doesn't support any extension. Erase all reuqired extensions.");
        extensions.clear();
        return AE_SUCCESS;
    }

    std::vector<VkExtensionProperties> prpos(count);
    ASSERT_VK_SUCCESS(vkEnumerateDeviceExtensionProperties(physical_device_, nullptr, &count, prpos.data()));

    auto it = extensions.begin();
    while (it != extensions.end()) {
        bool supported = false;
        for (const auto& prop : prpos) {
            if (strncmp(prop.extensionName, *it, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                LOG("VkDevice supports " + *it);
                supported = true;
                break;
            }
        }
        if (supported) {
            ++it;
        } else {
            LOG("VkDevice doesn't support " + *it + ". Erase it from the required list.");
            it = extensions.erase(it);
        }
    }
    return AE_SUCCESS;
}

Queues::Queues(const GPUObjectKey& key) : IGPUObject(AE_GPU_QUEUE) {}

AeResult Queues::initialize(const std::shared_ptr<Device>& device) {
    device_ = device;
    queue_flags_ = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    return AE_SUCCESS;
}

AeResult Queues::get_device_queue_create_infos(std::vector<VkDeviceQueueCreateInfo>& queue_cis) {
    GET_INSTANCE_PROC_ADDR(device_, GetPhysicalDeviceSurfaceSupportKHR);

    const auto physical_device = device_->get_VkPhysicalDevice();

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &count, nullptr);
    if (!count) {
        LOG("VkPhysicalDevice doesn't support any QueueFamily.");
        return AE_ERROR_UNKNOWN;
    }

    queue_family_props_.resize(count);
    for (auto& prop : queue_family_props_) {
        prop = {};
        prop.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }
    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &count, queue_family_props_.data());

    std::array<float, 100> queue_priorities;
    for (uint32_t i = 0; i < 100; ++i) {
        queue_priorities[i] = 1.f - 0.01f * i;
    }

    queue_family_support_surfaces_.resize(count);

    std::shared_ptr<Rendering> rendering = nullptr;
    ASSERT_SUCCESS(GPU_MGR.get<Rendering>(rendering));
    VkSurfaceKHR surface = rendering->get_VkSurfaceKHR();
    for (uint32_t i = 0; i < count; ++i) {
        ASSERT_VK_SUCCESS(fpGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &queue_family_support_surfaces_[i]));
    }

    for (uint32_t i = 0; i < count; ++i) {
        if ((queue_family_props_[i].queueFamilyProperties.queueFlags & queue_flags_) == queue_flags_) {
            uint32_t queue_index = 0;

            if (present_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)() && queue_family_support_surfaces_[i]) {
                present_queue_.family_index_ = i;
                present_queue_.queue_index_ = queue_index;

                main_queue_.family_index_ = i;
                main_queue_.queue_index_ = queue_index;
                ++queue_index;

                sub_queue_.family_index_ = i;
                sub_queue_.queue_index_ = queue_index;
                ++queue_index;
                break;
            }

            if (main_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
                main_queue_.family_index_ = i;
                main_queue_.queue_index_ = queue_index;
                ++queue_index;

                sub_queue_.family_index_ = i;
                sub_queue_.queue_index_ = queue_index;
                ++queue_index;
            }
        }
    }

    if (present_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        for (uint32_t i = 0; i < count; ++i) {
            if (queue_family_support_surfaces_[i]) {
                present_queue_.family_index_ = i;
                present_queue_.queue_index_ = 0;
                break;
            }
        }
    }

    if (main_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support main_queue_.");
        return AE_ERROR_UNKNOWN;
    }
    if (present_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support present_queue_.");
        return AE_ERROR_UNKNOWN;
    }
    if (sub_queue_.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support sub_queue_.");
        return AE_ERROR_UNKNOWN;
    }

    VkDeviceQueueCreateInfo queue_ci = {};
    queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_ci.queueFamilyIndex = sub_queue_.family_index_;
    queue_ci.queueCount = sub_queue_.queue_index_ + 1;
    queue_ci.pQueuePriorities = queue_priorities.data();
    queue_cis.emplace_back(queue_ci);

    if (present_queue_.family_index_ != queue_ci.queueFamilyIndex) {
        queue_ci.queueFamilyIndex = present_queue_.family_index_;
        queue_ci.queueCount = present_queue_.queue_index_ + 1;
        queue_cis.emplace_back(queue_ci);
    } else if (present_queue_.queue_index_ >= queue_cis[0].queueCount) {
        queue_cis[0].queueCount = present_queue_.queue_index_ + 1;
    }

    if (sub_queue_.queue_index_ >= queue_cis[0].queueCount) {
        queue_cis[0].queueCount = sub_queue_.queue_index_ + 1;
    }
    return AE_SUCCESS;
}

AeResult Queues::set_queues() {
    const auto device = device_->get_VkDevice();
    ASSERT_VK_NULL_HANDLE(device);

    VkDeviceQueueInfo2 queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    queue_info.queueFamilyIndex = main_queue_.family_index_;
    queue_info.queueIndex = main_queue_.queue_index_;
    vkGetDeviceQueue2(device, &queue_info, &main_queue_.queue_);
    ASSERT_VK_NULL_HANDLE(main_queue_.queue_);

    queue_info.queueFamilyIndex = present_queue_.family_index_;
    queue_info.queueIndex = present_queue_.queue_index_;
    vkGetDeviceQueue2(device, &queue_info, &present_queue_.queue_);
    ASSERT_VK_NULL_HANDLE(present_queue_.queue_);

    queue_info.queueFamilyIndex = sub_queue_.family_index_;
    queue_info.queueIndex = sub_queue_.queue_index_;
    vkGetDeviceQueue2(device, &queue_info, &sub_queue_.queue_);
    ASSERT_VK_NULL_HANDLE(sub_queue_.queue_);

    return AE_SUCCESS;
}

Rendering::Rendering(const GPUObjectKey& key) : IGPUObject(AE_GPU_RENDERING) {}

AeResult Rendering::initialize(const std::shared_ptr<Device>& device, const RenderingInfo& rendering_info) {
    device_ = device;
    rendering_info_ = rendering_info;
    return AE_SUCCESS;
}

AeResult Rendering::initialize_for_VkInstance() {
    ASSERT_SUCCESS(create_surface());
    return AE_SUCCESS;
}

AeResult Rendering::create_surface() {
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surface_ci = {};
    surface_ci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_ci.pNext = nullptr;
    surface_ci.flags = 0;
    surface_ci.hinstance = rendering_info_.present_info_.hinstance_;
    surface_ci.hwnd = rendering_info_.present_info_.hwnd_;
    const auto instance = device_->get_VkInstance();
    ASSERT_VK_NULL_HANDLE(instance);
    ASSERT_VK_SUCCESS(vkCreateWin32SurfaceKHR(instance, &surface_ci, nullptr, &present_.surface_));
#endif  // _WIN32

    return AE_SUCCESS;
}

AeResult Rendering::initialize_for_VkDevice() {
    GET_INSTANCE_PROC_ADDR(device_, GetPhysicalDeviceSurfaceCapabilities2KHR);
    GET_INSTANCE_PROC_ADDR(device_, GetPhysicalDeviceSurfaceFormats2KHR);
    GET_INSTANCE_PROC_ADDR(device_, GetPhysicalDeviceSurfacePresentModes2EXT);
    GET_DEVICE_PROC_ADDR(device_, CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(device_, DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(device_, GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(device_, AcquireNextImage2KHR);
    GET_DEVICE_PROC_ADDR(device_, QueuePresentKHR);

    ASSERT_SUCCESS(get_surface_property());
    ASSERT_SUCCESS(create_swapchain());
    return AE_SUCCESS;
}

AeResult Rendering::get_surface_property() {
    const auto physical_device = device_->get_VkPhysicalDevice();

    VkPhysicalDeviceSurfaceInfo2KHR surface_info = {};
    surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    surface_info.surface = present_.surface_;
    surface_capabilities_.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
    ASSERT_VK_SUCCESS(fpGetPhysicalDeviceSurfaceCapabilities2KHR(physical_device, &surface_info, &surface_capabilities_));

    uint32_t format_count = 0;
    ASSERT_VK_SUCCESS(fpGetPhysicalDeviceSurfaceFormats2KHR(physical_device, &surface_info, &format_count, nullptr));
    surface_formats_.resize(format_count);
    for (auto& format : surface_formats_) {
        format.sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
    }
    ASSERT_VK_SUCCESS(
        fpGetPhysicalDeviceSurfaceFormats2KHR(physical_device, &surface_info, &format_count, surface_formats_.data()));

    uint32_t mode_count = 0;
    ASSERT_VK_SUCCESS(fpGetPhysicalDeviceSurfacePresentModes2EXT(physical_device, &surface_info, &mode_count, nullptr));
    present_modes_.resize(mode_count);
    ASSERT_VK_SUCCESS(
        fpGetPhysicalDeviceSurfacePresentModes2EXT(physical_device, &surface_info, &mode_count, present_modes_.data()));

    return AE_SUCCESS;
}

AeResult Rendering::create_swapchain() {
    const auto device = device_->get_VkDevice();
    present_.old_swapchain_ = present_.swapchain_;

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.surface = present_.surface_;
    swapchain_ci.oldSwapchain = present_.old_swapchain_;

    ASSERT_VK_SUCCESS(fpCreateSwapchainKHR(device, &swapchain_ci, nullptr, &present_.swapchain_));
    return AE_SUCCESS;
}

VkSurfaceKHR Rendering::get_VkSurfaceKHR() { return present_.surface_; }

Manager::Manager() {}

AeResult Manager::initialize() { return AE_SUCCESS; }

AeResult Manager::cleanup() { return AE_SUCCESS; }

AeResult Manager::pre_update() { return AE_SUCCESS; }

AeResult Manager::update() { return AE_SUCCESS; }

AeResult Manager::post_update() { return AE_SUCCESS; }

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
