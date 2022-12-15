#ifndef AE_GPU_VULKAN_H
#define AE_GPU_VULKAN_H

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
BEGIN_NAMESPACE(vk)

OBJECT_KEY(VkObjectKey, Manager)

#define MANAGED_SINGLETON_VK_OBJECT(class_name) MANAGED_SINGLETON_OBJECT(class_name, VkObjectKey)
#define MANAGED_VK_OBJECT(class_name) MANAGED_OBJECT(class_name, VkObjectKey)

enum VkObjectType {
    VK_OBJECT_DEVICE = 0,
    VK_OBJECT_QUEUES = 1,
    VK_OBJECT_COMMAND_POOLS = 2,
    VK_OBJECT_RENDERING = 3,
    VK_OBJECT_IMAGE = 4,
    VK_OBJECT_RENDER_PASS = 5,

};

class IVkObject : public common::IObject {
   private:
    const VkObjectType type_;

   protected:
    IVkObject(const VkObjectType type) : common::IObject(), type_(type) {}

   public:
    IVkObject(const IVkObject &) = delete;
    IVkObject &operator=(const IVkObject &) = delete;

    VkObjectType get_type() { return type_; }

    virtual AeResult pre_update() { return AE_SUCCESS; }
    virtual AeResult update() { return AE_SUCCESS; }
    virtual AeResult post_update() { return AE_SUCCESS; }
};

class Queues;
class Rendering;
class Device : public IVkObject {
    MANAGED_SINGLETON_VK_OBJECT(Device)

   private:
    DeviceInfo device_info_{};

    std::shared_ptr<Queues> queues_{nullptr};
    AeResult initialize_queues();

    std::shared_ptr<Rendering> rendering_{nullptr};
    AeResult initialize_rendering(const RenderingInfo &rendering_info);

    AeResult initialize_command_pools();

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
    VkPhysicalDeviceProperties2 phy_dev_props_{};
    VkPhysicalDeviceFeatures2 phy_dev_feats_{};
    AeResult pick_physical_device();

    VkDevice device_{VK_NULL_HANDLE};
    AeResult create_device();
    AeResult check_support_device_extensions(std::vector<const char *> &extensions);

    std::unordered_map<VkFormat, VkFormatProperties2> format_props_;

   public:
    AeResult initialize(const InitializeInfo &initialize_info);
    AeResult check_support_format(const VkFormat format, const VkImageTiling tiling, const VkFormatFeatureFlags features);

    const VkInstance get_VkInstance();
    const VkPhysicalDevice get_VkPhysicalDevice();
    const VkDevice get_VkDevice();

    static VkBool32 debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
};

enum QueueType {
    QUEUE_MAIN = 0,
    QUEUE_PRESENT = 1,  // It could be the same with main_queue_ based on the GPU.
    QUEUE_SUB = 2,      // If the process could work individually without main_queue_.
    QUEUE_COUNT = 3,
};

struct Queue {
    uint32_t family_index_{(std::numeric_limits<uint32_t>::max)()};
    uint32_t queue_index_{0};
    VkQueue queue_{VK_NULL_HANDLE};
};

class Queues : public IVkObject {
    MANAGED_SINGLETON_VK_OBJECT(Queues)

   private:
    std::shared_ptr<Device> device_{nullptr};

    std::vector<VkQueueFamilyProperties2> queue_family_props_;
    std::vector<VkBool32> queue_family_support_surfaces_;
    const VkQueueFlags queue_flags_{VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT};

    std::array<Queue, QUEUE_COUNT> queues_;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;

   public:
    AeResult initialize(const std::shared_ptr<Device> &device);
    AeResult get_device_queue_create_infos(std::vector<VkDeviceQueueCreateInfo> &queue_cis);
    AeResult set_queues();
    const Queue *get_queue(QueueType type);
};

enum CommandType {
    COMMAND_MAIN = 0,
    COMMAND_PRESENT = 1,
    COMMAND_SUB = 2,
    COMMAND_COUNT = 3,
};

enum MainCommandType {
    MAIN_COMMAND_RENDERING = 0,
    MAIN_COMMAND_UPDATE = 1,
    MAIN_COMMAND_SUB = 2,
    MAIN_COMMAND_COUNT = 3,
};

enum PresentCommandType {
    PRESENT_COMMAND_MAIN = 0,
    PRESENT_COMMAND_COUNT = 1,
};

enum SubCommandType {
    SUB_COMMAND_MAIN = 0,
    SUB_COMMAND_COUNT = 1,
};

struct Command {
    uint32_t family_index_{0};
    VkCommandPool cmd_pool_{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> cmd_bufs_;
};

class CommandPools : public IVkObject {
    MANAGED_SINGLETON_VK_OBJECT(CommandPools)

   private:
    std::shared_ptr<Device> device_;

    std::array<Command, COMMAND_COUNT> cmds_;
    std::array<uint32_t, COMMAND_COUNT> cmd_buf_counts_{MAIN_COMMAND_COUNT, PRESENT_COMMAND_COUNT, SUB_COMMAND_COUNT};

   public:
    AeResult initialize(const std::shared_ptr<Device> &device);
    const VkCommandBuffer get_command_buffer(CommandType type, uint32_t index);
};

class Shader : public IVkObject {
    MANAGED_VK_OBJECT(Shader)

   private:
    VkShaderModule shader_{VK_NULL_HANDLE};
    // std::vector<ShaderBindingSlot> shader_data_slots_;
};

class IPipeline : public IVkObject {
   protected:
    VkPipeline pipeline_{VK_NULL_HANDLE};
};

class ComputePipeline : public IPipeline {
    MANAGED_VK_OBJECT(ComputePipeline)

   private:
    std::shared_ptr<Shader> compute_{nullptr};
};

class GraphicsPipeline : public IPipeline {
    MANAGED_VK_OBJECT(GraphicsPipeline)

   private:
    DrawingInfo drawing_info_{};

    std::shared_ptr<Shader> vertex_{nullptr};
    std::shared_ptr<Shader> tessellation_control_{nullptr};
    std::shared_ptr<Shader> tessellation_evaluation_{nullptr};
    std::shared_ptr<Shader> geometry_{nullptr};
    std::shared_ptr<Shader> fragment_{nullptr};

   public:
    bool compare_drawing_info(const DrawingInfo &drawing_info);
};

enum ImageType {
    IMAGE_SWAPCHAIN,
    IMAGE_DEPTH_STENCIL,
};

struct ImageInfo {
    ImageType type_{static_cast<ImageType>(0)};
    AeArray<uint32_t, 2> size_{0, 0}; // width, height
};

struct Image : public IVkObject {
    MANAGED_VK_OBJECT(Image)

   private:
    std::shared_ptr<Device> device_;
    ImageInfo image_info_{};

    VkImage image_{VK_NULL_HANDLE};
    VkDeviceMemory memory_{VK_NULL_HANDLE};
    VkImageView view_{VK_NULL_HANDLE};

    VkFormat get_format(const ImageType type);

   public:
    AeResult initialize(const std::shared_ptr<Device> &device, const ImageInfo &image_info);
    AeResult initialize_for_swapchain(const std::shared_ptr<Device> &device, const VkImage image, const VkFormat format);
};

enum RenderingLayerList {
    RENDERING_LAYER_PRESENT = 0,
    RENDERING_LAYER_UI = 1,
    RENDERING_LAYER_POSTPROCESSING = 2,
    RENDERING_LAYER_MAIN = 3,
    RENDERING_LAYER_OBJECTS = 4,
    RENDERING_LAYER_COUNT = 5,
};

class Framebuffer : public IVkObject {
    MANAGED_VK_OBJECT(Framebuffer)

   private:
    VkFramebuffer frambuffer_;
    // swapchain, depth, stencil;
};

class RenderPass : public IVkObject {
    MANAGED_VK_OBJECT(RenderPass)

   private:
    VkRenderPass render_pass_;
    // DrawCommand draw_command_;
};

// TODO: 1. Renderpass, 2: framebuffer, 3: rendering command, 4. present command, 5. sync,
//  6. graphics pipeline, 7. shader, 8. models, 9 descriptor.
//  When it's rendering, it runs every RenderingLayer, every RenderingLayer has models. Every model has a GraphicsPipeline. But Some
//  models could use the same GraphicsPipeline. When it addes a model, it checkes if or not it needs to create a new
//  GraphicsPipeline, and then when it's rendering, it will check if or not a model uses a different GraphicsPipeline to bind
//  another GraphicsPipeline. Complete one RenderingLayer first.
struct RenderingLayer {
    std::shared_ptr<Image> rendering_image_{nullptr};     // If it's present layer, this's swpachain image.
    std::shared_ptr<Image> depth_stenci_image_{nullptr};  // It's probably only for main layer.
    std::shared_ptr<Framebuffer> frambuffer_{nullptr};
    std::vector<std::shared_ptr<RenderPass>> render_passes_;
    // std::vector<Model> models_; // Every model has a GraphicsPipeline.
    // std::vector<std::shared_ptr<GraphicsPipeline>> graphics_pipelines_;
};

class Rendering : public IVkObject {
    MANAGED_SINGLETON_VK_OBJECT(Rendering)

   private:
    std::shared_ptr<Device> device_;
    RenderingInfo rendering_info_;

    // present
    VkSurfaceKHR surface_{VK_NULL_HANDLE};
    VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
    VkSwapchainKHR old_swapchain_{VK_NULL_HANDLE};
    const uint32_t desired_swapchain_image_count_{3};
    VkExtent2D present_size_{0, 0};
    std::vector<std::shared_ptr<Image>> swapchain_images_;
    uint32_t present_index_{0};

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

    std::array<RenderingLayer, RENDERING_LAYER_COUNT> rendering_layers_;

    AeResult create_depth_stencil_image(std::shared_ptr<Image> &image);
    AeResult create_renderpass();

   public:
    AeResult initialize_for_VkInstance(const std::shared_ptr<Device> &device, const RenderingInfo &rendering_info);
    AeResult initialize_for_VkDevice();
    VkSurfaceKHR get_VkSurfaceKHR();
};

class Resources : public IVkObject {};

/*
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
    INITIALIZE_MANAGER(Manager, VkObjectKey)
    MANAGE_GPU_OBJECT(Framebuffer)
    MANAGE_GPU_OBJECT(RenderPass)
    MANAGE_GPU_OBJECT(GraphicsPipeline)
    MANAGE_GPU_OBJECT(Image)

   public:
    AeResult pre_update();
    AeResult update();
    AeResult post_update();
};
#define MGR Manager::get_instance()

END_NAMESPACE(vk)
END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_VULKAN_H