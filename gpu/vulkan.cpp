#include <algorithm>
#include "vulkan.h"
#include "ui.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(vk)

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

Device::Device(const VkObjectKey& key)
    : IVkObject(VK_OBJECT_DEVICE), instance_(VK_NULL_HANDLE), physical_device_(VK_NULL_HANDLE), device_(VK_NULL_HANDLE) {}

const VkInstance Device::get_VkInstance() { return instance_; }
const VkPhysicalDevice Device::get_VkPhysicalDevice() { return physical_device_; }
const VkDevice Device::get_VkDevice() { return device_; }

AeResult Device::initialize(const InitializeInfo& initialize_info) {
    device_info_ = initialize_info.device_info_;

    ASSERT_SUCCESS(create_instance());
    ASSERT_SUCCESS(initialize_queues());
    ASSERT_SUCCESS(initialize_rendering(initialize_info.rendering_info_));
    ASSERT_SUCCESS(pick_physical_device());
    ASSERT_SUCCESS(create_device());
    ASSERT_SUCCESS(queues_->set_queues());
    ASSERT_SUCCESS(initialize_command_pools());
    ASSERT_SUCCESS(rendering_->initialize_for_VkDevice());

    return AE_SUCCESS;
}

AeResult Device::initialize_queues() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(MGR.get<Device>(device));
    ASSERT_SUCCESS(MGR.get<Queues>(queues_));
    ASSERT_SUCCESS(queues_->initialize(device));
    return AE_SUCCESS;
}

AeResult Device::initialize_rendering(const RenderingInfo& rendering_info) {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(MGR.get<Device>(device));
    ASSERT_SUCCESS(MGR.get<Rendering>(rendering_));
    ASSERT_SUCCESS(rendering_->initialize_for_VkInstance(device, rendering_info));
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

    phy_dev_props_ = {};
    phy_dev_props_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(physical_device_, &phy_dev_props_);

    LOG("Physical device: " + phy_dev_props_.properties.deviceName +
        ", driver: " + (phy_dev_props_.properties.driverVersion >> 22) + "." +
        ((phy_dev_props_.properties.driverVersion >> 12) & 0x3ff) + "." + (phy_dev_props_.properties.driverVersion & 0xfff) +
        ", API Version: " + VK_API_VERSION_VARIANT(phy_dev_props_.properties.apiVersion) + "." +
        VK_API_VERSION_MAJOR(phy_dev_props_.properties.apiVersion) + "." +
        VK_API_VERSION_MINOR(phy_dev_props_.properties.apiVersion) + "." +
        VK_API_VERSION_PATCH(phy_dev_props_.properties.apiVersion));

    phy_dev_feats_ = {};
    phy_dev_feats_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(physical_device_, &phy_dev_feats_);
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

AeResult Device::initialize_command_pools() {
    std::shared_ptr<Device> device;
    std::shared_ptr<CommandPools> command_pools;
    ASSERT_SUCCESS(MGR.get<Device>(device));
    ASSERT_SUCCESS(MGR.get<CommandPools>(command_pools));
    ASSERT_SUCCESS(command_pools->initialize(device));
    return AE_SUCCESS;
}

AeResult Device::check_support_format(const VkFormat format, const VkImageTiling tiling, const VkFormatFeatureFlags features) {
    auto it = format_props_.find(format);
    if (it == format_props_.end()) {
        VkFormatProperties2 props;
        vkGetPhysicalDeviceFormatProperties2(physical_device_, format, &props);
        auto ret = format_props_.insert({format, props});
        it = ret.first;
    }
    if (tiling == VK_IMAGE_TILING_LINEAR && (it->second.formatProperties.linearTilingFeatures & features) == features) {
        return AE_SUCCESS;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (it->second.formatProperties.optimalTilingFeatures & features) == features) {
        return AE_SUCCESS;
    }
    return AE_ERROR_UNKNOWN;
}

Queues::Queues(const VkObjectKey& key) : IVkObject(VK_OBJECT_QUEUES) {}

AeResult Queues::initialize(const std::shared_ptr<Device>& device) {
    device_ = device;
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
    ASSERT_SUCCESS(MGR.get<Rendering>(rendering));
    VkSurfaceKHR surface = rendering->get_VkSurfaceKHR();
    for (uint32_t i = 0; i < count; ++i) {
        ASSERT_VK_SUCCESS(fpGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &queue_family_support_surfaces_[i]));
    }

    auto& main_queue = queues_[QUEUE_MAIN];
    auto& present_queue = queues_[QUEUE_PRESENT];
    auto& sub_queue = queues_[QUEUE_SUB];

    for (uint32_t i = 0; i < count; ++i) {
        if ((queue_family_props_[i].queueFamilyProperties.queueFlags & queue_flags_) == queue_flags_) {
            uint32_t queue_index = 0;

            if (present_queue.family_index_ == (std::numeric_limits<uint32_t>::max)() && queue_family_support_surfaces_[i]) {
                present_queue.family_index_ = i;
                present_queue.queue_index_ = queue_index;

                main_queue.family_index_ = i;
                main_queue.queue_index_ = queue_index;
                ++queue_index;

                sub_queue.family_index_ = i;
                sub_queue.queue_index_ = queue_index;
                ++queue_index;
                break;
            }

            if (main_queue.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
                main_queue.family_index_ = i;
                main_queue.queue_index_ = queue_index;
                ++queue_index;

                sub_queue.family_index_ = i;
                sub_queue.queue_index_ = queue_index;
                ++queue_index;
            }
        }
    }

    if (present_queue.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        for (uint32_t i = 0; i < count; ++i) {
            if (queue_family_support_surfaces_[i]) {
                present_queue.family_index_ = i;
                present_queue.queue_index_ = 0;
                break;
            }
        }
    }

    if (main_queue.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support main_queue_.");
        return AE_ERROR_UNKNOWN;
    }
    if (present_queue.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support present_queue_.");
        return AE_ERROR_UNKNOWN;
    }
    if (sub_queue.family_index_ == (std::numeric_limits<uint32_t>::max)()) {
        LOG("VkPhysicalDevice doesn't support sub_queue_.");
        return AE_ERROR_UNKNOWN;
    }

    VkDeviceQueueCreateInfo queue_ci = {};
    queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_ci.queueFamilyIndex = sub_queue.family_index_;
    queue_ci.queueCount = sub_queue.queue_index_ + 1;
    queue_ci.pQueuePriorities = queue_priorities.data();
    queue_cis.emplace_back(queue_ci);

    if (present_queue.family_index_ != queue_ci.queueFamilyIndex) {
        queue_ci.queueFamilyIndex = present_queue.family_index_;
        queue_ci.queueCount = present_queue.queue_index_ + 1;
        queue_cis.emplace_back(queue_ci);
    } else if (present_queue.queue_index_ >= queue_cis[0].queueCount) {
        queue_cis[0].queueCount = present_queue.queue_index_ + 1;
    }

    if (sub_queue.queue_index_ >= queue_cis[0].queueCount) {
        queue_cis[0].queueCount = sub_queue.queue_index_ + 1;
    }
    return AE_SUCCESS;
}

AeResult Queues::set_queues() {
    const auto device = device_->get_VkDevice();
    ASSERT_VK_NULL_HANDLE(device);

    VkDeviceQueueInfo2 queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;

    for (auto& queue : queues_) {
        queue_info.queueFamilyIndex = queue.family_index_;
        queue_info.queueIndex = queue.queue_index_;
        vkGetDeviceQueue2(device, &queue_info, &queue.queue_);
        ASSERT_VK_NULL_HANDLE(queue.queue_);
    }

    return AE_SUCCESS;
}

const Queue* Queues::get_queue(QueueType type) {
    if (type >= QUEUE_COUNT) return nullptr;
    return &queues_[type];
}

CommandPools::CommandPools(const VkObjectKey& key) : IVkObject(VK_OBJECT_COMMAND_POOLS) {}

AeResult CommandPools::initialize(const std::shared_ptr<Device>& device) {
    device_ = device;
    VkDevice vk_device = device_->get_VkDevice();

    ASSERT(QUEUE_COUNT == COMMAND_COUNT, "");

    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(MGR.get<Queues>(queues));

    VkCommandPoolCreateInfo cmd_pool_ci = {};
    cmd_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandBufferAllocateInfo cmd_ai = {};
    cmd_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    uint32_t main_queue_family_index = 0;
    VkCommandPool main_cmd_pool = VK_NULL_HANDLE;

    for (uint32_t i = 0; i < COMMAND_COUNT; ++i) {
        const auto* queue = queues->get_queue(static_cast<QueueType>(i));
        ASSERT_NULL(queue);

        auto& cmd = cmds_[i];
        cmd.family_index_ = queue->family_index_;

        cmd_pool_ci.queueFamilyIndex = cmd.family_index_;
        if (i == QUEUE_PRESENT && queue->family_index_ == main_queue_family_index) {
            cmd.cmd_pool_ = main_cmd_pool;
        } else {
            ASSERT_VK_SUCCESS(vkCreateCommandPool(vk_device, &cmd_pool_ci, NULL, &cmd.cmd_pool_));
        }

        if (i == QUEUE_MAIN) {
            main_queue_family_index = queue->family_index_;
            main_cmd_pool = cmd.cmd_pool_;
        }

        cmd.cmd_bufs_.resize(cmd_buf_counts_[i]);
        cmd_ai.commandBufferCount = cmd_buf_counts_[i];
        cmd_ai.commandPool = cmd.cmd_pool_;
        ASSERT_VK_SUCCESS(vkAllocateCommandBuffers(vk_device, &cmd_ai, cmd.cmd_bufs_.data()));
    }
    return AE_SUCCESS;
}

const VkCommandBuffer CommandPools::get_command_buffer(CommandType type, uint32_t index) {
    if (type >= COMMAND_COUNT) return VK_NULL_HANDLE;
    if (index >= cmd_buf_counts_[type]) return VK_NULL_HANDLE;
    return cmds_[type].cmd_bufs_[index];
}

Image::Image(const VkObjectKey& key) : IVkObject(VK_OBJECT_IMAGE) {}

AeResult Image::initialize(const std::shared_ptr<Device>& device, const ImageInfo& image_info) {
    device_ = device;
    image_info_ = image_info;
    const auto vk_device = device_->get_VkDevice();

    VkImageCreateInfo image_ci = {};
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.imageType = VK_IMAGE_TYPE_2D;
    image_ci.format = get_format(image_info_.type_);
    image_ci.extent.width = image_info_.size_.width;
    image_ci.extent.height = image_info_.size_.height;
    image_ci.mipLevels = 1;
    image_ci.arrayLayers = 1;
    image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
    image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_ci.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ASSERT_VK_SUCCESS(vkCreateImage(vk_device, &image_ci, nullptr, &image_));

    VkImageViewCreateInfo view_ci = {};
    view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ASSERT_VK_SUCCESS(vkCreateImageView(vk_device, &view_ci, nullptr, &view_));
    return AE_SUCCESS;
}

VkFormat Image::get_format(const ImageType type) {
    switch (type) {
        case IMAGE_SWAPCHAIN:
            return VK_FORMAT_UNDEFINED;
        case IMAGE_DEPTH_STENCIL: {
            std::array<VkFormat, 3> formats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
            for (auto format : formats) {
                if (VK_SUCCESS == device_->check_support_format(format, VK_IMAGE_TILING_OPTIMAL,
                                                                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
                    return format;
                }
            }
            return VK_FORMAT_UNDEFINED;
        }
        default:
            return VK_FORMAT_UNDEFINED;
    }
    return VK_FORMAT_UNDEFINED;
}

AeResult Image::initialize_for_swapchain(const std::shared_ptr<Device>& device, const VkImage image, const VkFormat format) {
    device_ = device;
    VkDevice vk_device = device_->get_VkDevice();
    image_ = image;
    image_info_.type_ = IMAGE_SWAPCHAIN;

    VkImageViewCreateInfo view_ci = {};
    view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_ci.format = format;
    view_ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_ci.subresourceRange.baseMipLevel = 0;
    view_ci.subresourceRange.levelCount = 1;
    view_ci.subresourceRange.baseArrayLayer = 0;
    view_ci.subresourceRange.layerCount = 1;
    view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_ci.image = image;

    ASSERT_VK_SUCCESS(vkCreateImageView(vk_device, &view_ci, nullptr, &view_));
    return AE_SUCCESS;
}

Rendering::Rendering(const VkObjectKey& key) : IVkObject(VK_OBJECT_RENDERING) {}

AeResult Rendering::initialize_for_VkInstance(const std::shared_ptr<Device>& device, const RenderingInfo& rendering_info) {
    device_ = device;
    rendering_info_ = rendering_info;
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
    ASSERT_VK_SUCCESS(vkCreateWin32SurfaceKHR(instance, &surface_ci, nullptr, &surface_));
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
    ASSERT_SUCCESS(create_renderpass());
    return AE_SUCCESS;
}

AeResult Rendering::get_surface_property() {
    const auto physical_device = device_->get_VkPhysicalDevice();

    VkPhysicalDeviceSurfaceInfo2KHR surface_info = {};
    surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    surface_info.surface = surface_;
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
    old_swapchain_ = swapchain_;

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.surface = surface_;
    swapchain_ci.oldSwapchain = old_swapchain_;
    swapchain_ci.imageFormat = surface_formats_[0].surfaceFormat.format;
    swapchain_ci.imageColorSpace = surface_formats_[0].surfaceFormat.colorSpace;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.clipped = VK_TRUE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = nullptr;

    auto& present_mode = swapchain_ci.presentMode;
    present_mode = present_modes_[0];
    if (present_mode != VK_PRESENT_MODE_FIFO_KHR) {
        for (const auto& mode : present_modes_) {
            if (mode == VK_PRESENT_MODE_FIFO_KHR) {
                present_mode = VK_PRESENT_MODE_FIFO_KHR;
                break;
            }
        }
    }

    auto& min_image_count = swapchain_ci.minImageCount;
    min_image_count = desired_swapchain_image_count_;
    if (min_image_count < surface_capabilities_.surfaceCapabilities.minImageCount) {
        min_image_count = surface_capabilities_.surfaceCapabilities.minImageCount;
    }
    if ((surface_capabilities_.surfaceCapabilities.maxImageCount > 0) &&
        (min_image_count > surface_capabilities_.surfaceCapabilities.maxImageCount)) {
        min_image_count = surface_capabilities_.surfaceCapabilities.maxImageCount;
    }

    auto& image_extent = swapchain_ci.imageExtent;
    if (surface_capabilities_.surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        AeXMLNode* node = CONFIG->getXMLNode("setting.environment");
        image_extent.width = node->getXMLValue<int>("mainWidth");
        image_extent.height = node->getXMLValue<int>("mainHeight");

        if (image_extent.width < surface_capabilities_.surfaceCapabilities.minImageExtent.width) {
            image_extent.width = surface_capabilities_.surfaceCapabilities.minImageExtent.width;
        } else if (image_extent.width > surface_capabilities_.surfaceCapabilities.maxImageExtent.width) {
            image_extent.width = surface_capabilities_.surfaceCapabilities.maxImageExtent.width;
        }
        if (image_extent.height < surface_capabilities_.surfaceCapabilities.minImageExtent.height) {
            image_extent.height = surface_capabilities_.surfaceCapabilities.minImageExtent.height;
        } else if (image_extent.height > surface_capabilities_.surfaceCapabilities.maxImageExtent.height) {
            image_extent.height = surface_capabilities_.surfaceCapabilities.maxImageExtent.height;
        }
    } else {
        image_extent = surface_capabilities_.surfaceCapabilities.currentExtent;
    }
    present_size_ = image_extent;

    auto& pre_transform = swapchain_ci.preTransform;
    if (surface_capabilities_.surfaceCapabilities.currentTransform > 0) {
        pre_transform = surface_capabilities_.surfaceCapabilities.currentTransform;
    } else {
        for (uint32_t i = 0; i < 9; ++i) {
            if ((surface_capabilities_.surfaceCapabilities.supportedTransforms >> i) & 1) {
                pre_transform = static_cast<VkSurfaceTransformFlagBitsKHR>(1 << i);
                break;
            }
        }
    }

    auto& composite_alpha = swapchain_ci.compositeAlpha;
    for (uint32_t i = 0; i < 4; ++i) {
        if ((surface_capabilities_.surfaceCapabilities.supportedCompositeAlpha >> i) & 1) {
            composite_alpha = static_cast<VkCompositeAlphaFlagBitsKHR>(1 << i);
            break;
        }
    }
    ASSERT_VK_SUCCESS(fpCreateSwapchainKHR(device, &swapchain_ci, nullptr, &swapchain_));

    std::vector<VkImage> swapchain_images;
    uint32_t count = 0;
    ASSERT_VK_SUCCESS(fpGetSwapchainImagesKHR(device, swapchain_, &count, nullptr));
    swapchain_images.resize(count);
    ASSERT_VK_SUCCESS(fpGetSwapchainImagesKHR(device, swapchain_, &count, swapchain_images.data()));

    for (auto swapchain_image : swapchain_images) {
        std::shared_ptr<Image> image;
        ASSERT_SUCCESS(MGR.create<Image>(image));
        swapchain_images_.emplace_back(image);
        ASSERT_SUCCESS(image->initialize_for_swapchain(device_, swapchain_image, surface_formats_[0].surfaceFormat.format));
    }
    return AE_SUCCESS;
}

VkSurfaceKHR Rendering::get_VkSurfaceKHR() { return surface_; }

AeResult Rendering::create_depth_stencil_image(std::shared_ptr<Image>& depth_stencil_image) {
    std::shared_ptr<Image> depth_stenci_image;
    ASSERT_SUCCESS(MGR.create<Image>(depth_stenci_image));

    ImageInfo image_info = {};
    image_info.type_ = IMAGE_DEPTH_STENCIL;
    image_info.size_.width = present_size_.width;
    image_info.size_.height = present_size_.height;

    depth_stencil_image->initialize(device_, image_info);
    return AE_SUCCESS;
}

// TODO: Main first
AeResult Rendering::create_renderpass() {
    auto& layer = rendering_layers_[RENDERING_LAYER_MAIN];
    ASSERT_SUCCESS(create_depth_stencil_image(layer.depth_stenci_image_));

    std::shared_ptr<RenderPass> render_pass;
    ASSERT_SUCCESS(MGR.create<RenderPass>(render_pass));

    layer.render_passes_.emplace_back(render_pass);
    return AE_SUCCESS;
}

RenderPass::RenderPass(const VkObjectKey& key) : IVkObject(VK_OBJECT_RENDER_PASS) {}

Manager::Manager() {}

AeResult Manager::initialize() { return AE_SUCCESS; }

AeResult Manager::cleanup() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(get<Device>(device));
    ASSERT_SUCCESS(device->cleanup());

    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(get<Queues>(queues));
    ASSERT_SUCCESS(queues->cleanup());

    std::shared_ptr<CommandPools> cmd_pools;
    ASSERT_SUCCESS(get<CommandPools>(cmd_pools));
    ASSERT_SUCCESS(cmd_pools->cleanup());

    std::shared_ptr<Rendering> rendering;
    ASSERT_SUCCESS(get<Rendering>(rendering));
    ASSERT_SUCCESS(rendering->cleanup());
    return AE_SUCCESS;
}

AeResult Manager::pre_update() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(get<Device>(device));
    ASSERT_SUCCESS(device->pre_update());

    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(get<Queues>(queues));
    ASSERT_SUCCESS(queues->pre_update());

    std::shared_ptr<CommandPools> cmd_pools;
    ASSERT_SUCCESS(get<CommandPools>(cmd_pools));
    ASSERT_SUCCESS(cmd_pools->pre_update());

    std::shared_ptr<Rendering> rendering;
    ASSERT_SUCCESS(get<Rendering>(rendering));
    ASSERT_SUCCESS(rendering->pre_update());
    return AE_SUCCESS;
}

AeResult Manager::update() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(get<Device>(device));
    ASSERT_SUCCESS(device->update());

    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(get<Queues>(queues));
    ASSERT_SUCCESS(queues->update());

    std::shared_ptr<CommandPools> cmd_pools;
    ASSERT_SUCCESS(get<CommandPools>(cmd_pools));
    ASSERT_SUCCESS(cmd_pools->update());

    std::shared_ptr<Rendering> rendering;
    ASSERT_SUCCESS(get<Rendering>(rendering));
    ASSERT_SUCCESS(rendering->update());
    return AE_SUCCESS;
}

AeResult Manager::post_update() {
    std::shared_ptr<Device> device;
    ASSERT_SUCCESS(get<Device>(device));
    ASSERT_SUCCESS(device->post_update());

    std::shared_ptr<Queues> queues;
    ASSERT_SUCCESS(get<Queues>(queues));
    ASSERT_SUCCESS(queues->post_update());

    std::shared_ptr<CommandPools> cmd_pools;
    ASSERT_SUCCESS(get<CommandPools>(cmd_pools));
    ASSERT_SUCCESS(cmd_pools->post_update());

    std::shared_ptr<Rendering> rendering;
    ASSERT_SUCCESS(get<Rendering>(rendering));
    ASSERT_SUCCESS(rendering->post_update());
    return AE_SUCCESS;
}

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)
