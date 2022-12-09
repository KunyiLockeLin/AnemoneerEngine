#ifndef AE_GPU_GPU_OBJECTS_H
#define AE_GPU_GPU_OBJECTS_H

#include <unordered_map>
#include <memory>
#include <list>

#ifdef _WIN32
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // _WIN32

#include <vulkan/vulkan.hpp>

#include "gpu.h"  // TODO: Maybe here shouldn't use gpu.h. gpu.h is interface of here.

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)
BEGIN_NAMESPACE(object)

OBJECT_KEY(GPUObjectKey, Manager)

#define MANAGED_SINGLETON_GPU_OBJECT(class_name) MANAGED_SINGLETON_OBJECT(class_name, GPUObjectKey)
#define MANAGED_GPU_OBJECT(class_name) MANAGED_OBJECT(class_name, GPUObjectKey)

class IGPUObject : public common::IObject {
   private:
    const AeGPUType type_;

   protected:
    IGPUObject(const AeGPUType type) : common::IObject(), type_(type) {}

   public:
    IGPUObject(const IGPUObject &) = delete;
    IGPUObject &operator=(const IGPUObject &) = delete;

    AeGPUType get_type() { return type_; }

    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Queues;
class Rendering;
class Device : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Device)

   private:
    DeviceInfo device_info_{};

    std::shared_ptr<Queues> queues_{nullptr};
    AeResult initialize_queues();

    std::shared_ptr<Rendering> rendering_{nullptr};
    AeResult initialize_rendering(const RenderingInfo &rendering_info);

    uint32_t instance_api_version_{0};
    VkInstance instance_{VK_NULL_HANDLE};
    AeResult create_instance();
    AeResult check_support_instance_layers(std::vector<const char *> &layers);
    AeResult check_support_instance_extensions(std::vector<const char *> &extensions);

    VkDebugUtilsMessengerEXT debug_messenger_{VK_NULL_HANDLE};
    PFN_vkCreateDebugUtilsMessengerEXT fpCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT fpDestroyDebugUtilsMessengerEXT;
    AeResult create_debug_messenger_callback();

    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
    VkPhysicalDeviceProperties2 phy_dev_props2_{};
    VkPhysicalDeviceFeatures2 phy_dev_feats2_{};
    AeResult pick_physical_device();

    VkDevice device_{VK_NULL_HANDLE};
    AeResult create_device();
    AeResult check_support_device_extensions(std::vector<const char *> &extensions);

   public:
    AeResult initialize(const InitializeInfo &initialize_info);
    const VkInstance get_VkInstance();
    const VkPhysicalDevice get_VkPhysicalDevice();
    const VkDevice get_VkDevice();

    static VkBool32 debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
};

class Queues : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Queues)

   private:
    std::shared_ptr<Device> device_{nullptr};

    std::vector<VkQueueFamilyProperties2> queue_family_props_;
    std::vector<VkBool32> queue_family_support_surfaces_;

    VkQueueFlags queue_flags_{0};  // VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT.

    struct Queue {
        uint32_t family_index_{(std::numeric_limits<uint32_t>::max)()};
        uint32_t queue_index_{0};
        VkQueue queue_{VK_NULL_HANDLE};
    };

    enum QueueType {
        QUEUE_MAIN,
        QUEUE_PRESENT,
        QUEUE_SUB,
    };

    Queue main_queue_;
    Queue present_queue_;  // It could be the same with main_queue_ based on the GPU.
    Queue sub_queue_;      // If the process could work individually without main_queue_;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;

   public:
    AeResult initialize(const std::shared_ptr<Device> &device);
    AeResult get_device_queue_create_infos(std::vector<VkDeviceQueueCreateInfo> &queue_cis);
    AeResult set_queues();
};

class CommandPools : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(CommandPools)

   private:
    std::shared_ptr<Device> device_;

    // AeResult create_command_pools();
    // AeResult allocate_command_buffers(CommandPoolType target_commamd_pool, uint32_t cmd_buf_count, VkCommandBuffer *cmd_bufs);

   public:
    AeResult initialize(const std::shared_ptr<Device> &device);
};

class Framebuffer;
class RenderPass;
class GraphicsPipeline;
class Rendering : public IGPUObject {
    MANAGED_SINGLETON_GPU_OBJECT(Rendering)

   private:
    std::shared_ptr<Device> device_;
    RenderingInfo rendering_info_;

    struct Present {
        VkSurfaceKHR surface_{VK_NULL_HANDLE};
        VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
        VkSwapchainKHR old_swapchain_{VK_NULL_HANDLE};
        uint32_t present_index_{0};
        std::vector<VkImage> swapchain_images_;
    } present_;

    VkSurfaceCapabilities2KHR surface_capabilities_{};
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR fpGetPhysicalDeviceSurfaceCapabilities2KHR;

    std::vector<VkSurfaceFormat2KHR> surface_formats_;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR fpGetPhysicalDeviceSurfaceFormats2KHR;

    std::vector<VkPresentModeKHR> present_modes_;
    PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT fpGetPhysicalDeviceSurfacePresentModes2EXT;

    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    PFN_vkAcquireNextImage2KHR fpAcquireNextImage2KHR;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;

    AeResult create_surface();
    AeResult get_surface_property();
    AeResult create_swapchain();

    struct RenderingLayer {
        AE_RENDER_TYPE render_Type;
        std::shared_ptr<Framebuffer> frambuffer_;
        std::vector<std::shared_ptr<RenderPass>> render_passes_;
        std::vector<std::shared_ptr<GraphicsPipeline>> graphics_pipelines_;
    };
    std::vector<RenderingLayer> rendering_layers_;

   public:
    AeResult initialize(const std::shared_ptr<Device> &device, const RenderingInfo &rendering_info);
    AeResult initialize_for_VkInstance();
    AeResult initialize_for_VkDevice();
    VkSurfaceKHR get_VkSurfaceKHR();
};

class Framebuffer : public IGPUObject {
    MANAGED_GPU_OBJECT(Framebuffer)

   private:
    VkFramebuffer frambuffer_;
    // swapchain, depth, stencil;
};

class RenderPass : public IGPUObject {
    MANAGED_GPU_OBJECT(RenderPass)

   private:
    VkRenderPass render_pass_;
    // DrawCommand draw_command_;
};

class IPipeline : public IGPUObject {
   protected:
    VkPipeline pipeline_;
    std::vector<VkShaderModule> shader_modules_;
    // std::vector<ShaderBindingSlot> shader_data_slots_;
};

class GraphicsPipeline : public IPipeline {
    MANAGED_GPU_OBJECT(GraphicsPipeline)

   private:
};
/*
class Command : public IGPUObject {
   protected:
    VkCommandBuffer command_buffer_;
};

class DrawCommand : public Command {
    MANAGED_GPU_OBJECT(DrawCommand )
};

class Thread : public IGPUObject {};

class Memory : public IGPUObject {
   protected:
};

class Model : public Memory {
    MANAGED_GPU_OBJECT(Model)

   private:
    VkBuffer buffer_;
    ModelInfo model_info_;

   public:
    virtual AeResult initialize(ModelInfo &model_info);
    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};


class Texture : public Memory {
   private:
    VkImage image_;
    VkImageView image_view_;
};


class Computer : public IPipeline {
   private:
    // computer pipeline
};
*/
#define MANAGE_GPU_OBJECT(class_name) MANAGE_OBJECT(Manager, ID, class_name)

// TODO: It should have a memory allocator.The memory allocator will create a block empty memory.
//       When a manager needs a new object, it will require a size memory from the memory allocator,
//       and then convert it to the target class. When a object is released, This block memory will
//       be set unused in the memory allocator. This unused memory could be used again.
class Manager {
    INITIALIZE_MANAGER(Manager, GPUObjectKey)
    MANAGE_GPU_OBJECT(Framebuffer)
    MANAGE_GPU_OBJECT(RenderPass)
    MANAGE_GPU_OBJECT(GraphicsPipeline)

   public:
    AeResult pre_update();
    AeResult update();
    AeResult post_update();
};
#define GPU_MGR Manager::get_instance()

END_NAMESPACE(object)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_GPU_OBJECTS_H
