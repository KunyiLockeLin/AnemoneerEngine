#ifndef AE_GPU_GPU_H
#define AE_GPU_GPU_H

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <limits>

#include "common/common.h"
#include "common/object.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(gpu)

struct PresentInfo {
// WSI(window system integration)
#ifdef _WIN32
    HINSTANCE hinstance_ = NULL;
    HWND hwnd_ = NULL;
#endif  // _WIN32
};

struct RenderingInfo {
    PresentInfo present_info_{};
};

// Setting graphics pipeline. Every model needs one.
struct DrawingInfo {
    std::string vertex_shader_path_{""};
    std::string tessellation_control_shader_path_{""};
    std::string tessellation_evaluation_shader_path_{""};
    std::string geometry_shader_path_{""};
    std::string fragment_shader_path_{""};
};

struct TransformInfo {
    AeArray<float, 3> position;
    AeArray<float, 3> scale;
    AeArray<float, 3> faceEular;
    AeArray<float, 3> rotateSpeed;
    AeArray<float, 3> revoluteSpeed;
    AeArray<bool, 3> revoluteFixAxis;
};

struct TriangleInfo {
    DrawingInfo drawing_info_;
    TransformInfo transformInfo_;
};

struct CubeInfo {
    DrawingInfo drawing_info_;
    TransformInfo transformInfo_;
};

struct ModelInfo {
    DrawingInfo drawing_info_;
    TransformInfo transformInfo_;

    std::string model_path_{""};
};

class IRendering {
   public:
};

struct DeviceInfo {
    std::vector<const char*> layer_names_;
    uint32_t gpu_index_{(std::numeric_limits<uint32_t>::max)()};  // max means the last GPU devices.
};

class IDevice {
   public:
    // Only support single Rednering. If it supports plural, it will become create_rendering.
    IRendering* get_rendering();
};

struct InitializeInfo {
    DeviceInfo device_info_{};
    RenderingInfo rendering_info_{}; 
};

// Only support single GPU and single device. If it supports plural, it will become create_device.
AeResult initialize(const struct InitializeInfo& initialize_info, IDevice*& device);
AeResult cleanup();
AeResult pre_update();
AeResult update();
AeResult post_update();

struct DrawPointData {};
struct DrawLineData {};
struct DrawTriangleData {};
struct DrawModelData {};
struct TextureData {};
struct ShaderBindingSlot {};
struct RenderPass {};

END_NAMESPACE(gpu)
END_NAMESPACE(ae)

#endif  // AE_GPU_GPU_H
