#pragma once

#define NOMINMAX

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <thread>
#include <memory>
#include <commctrl.h>
#include <utility>
#include <conio.h>
#include <algorithm>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#define KEY_FSLASH 0x2F
#define KEY_A 0x41
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_W 0x57
//#define KEY_W 229
#define KEY_L 0x4C
#define KEY_X 0x58
#define KEY_Z 0x5A

// VkPhysicalDeviceLimits::maxViewports
// VK->deviceProperties.limits.maxViewports;
const int MAX_JOINT_NUM = 20;
const char BONE_ROOT_NAME[] = "Armature_root";
const int MAX_DESCRIPTOR_UNIFORM_NUM = UINT8_MAX;
const int MAX_DESCRIPTOR_SAMPLER_NUM = UINT8_MAX;
const int MAX_DESCRIPTOR_INPUTATTACH_NUM = UINT8_MAX;
const int MAX_DESCRIPTOR_STORAGETEXEL_NUM = UINT8_MAX;
const int MAX_DESCRIPTOR_STORAG_NUM = UINT8_MAX;
const int MAX_DESCRIPTOR_NUM = UINT8_MAX;
const int MAX_PARTICLES_NUM = UINT16_MAX;
const int PUSH_CONSTANTS_SIZE = 4;

struct QeVKBuffer;
struct QeVKImageBuffer;
struct QeVertex;
struct QeAssetModel;
struct QeAssetMaterial;
struct QeAssetParticleRule;
struct QeDataMaterial;
struct QeDataDescriptorSet;
struct QeAssetImage;
struct QeAssetGraphicsShader;
struct QeUniformBufferObject;
struct QeDataLight;
struct QeSkeleton;
struct QeInputData;
struct QeDataSwapchain;
struct QeDataDrawCommand;
class QeObject;
class QeComponent;
class QeTransform;
class QeInputControl;
class QePostProcessing;
class QeGameAsset;
class QeModel;
class QePlane;
class QeCubemap;
class QeParticle;
class QeCompute;
class QeCamera;
class QeLight;
class AeGlobalKey;
class AeGlobal;
class AeUI;
class QeViewport;
class AngryEngine;
class AeObjectManagerKey;
class AeObjectManager;
class QeVulkan;
class QeCommand;
class QeLine;
class QeAxis;
class QeGrid;
class QeRenderSetting;

enum QeModelDataType {
    eModelData_cube = 0,
    eModelData_axis = 1,
    eModelData_grid = 2,
    eModelData_line = 3,
    eModelData_plane = 4,
    eModelData_gltf = 5,
};

enum QeGameAssetType {
    eAssetModel = 0,
    eAssetMaterial = 1,
    eAssetBin = 2,
    eAssetShader = 3,
    eAssetTexture = 4,
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

enum QeVKImageType {
    eImage_depthStencil = 0,
    eImage_swapchain = 1,
    eImage_attach = 2,
    eImage_2D = 3,
    eImage_cube = 4,
    eImage_render = 5,
    eImage_storage = 6,
};

enum QeDescriptorSetLayoutType {
    eDescriptorSetLayout_Model = 0,
    eDescriptorSetLayout_Common = 1,
    eDescriptorSetLayout_Postprocessing = 2,
    eDescriptorSetLayout_Compute = 3,
    eDescriptorSetLayout_Raytracing = 4,
};

enum QeGraphicsPipelineOtherType {
    eGraphicsPipeLine_none = 0,
    eGraphicsPipeLine_normal = 1,
    eGraphicsPipeLine_stencilBuffer = 2,
};

enum QeUIType {
    eUIType_btnPause = 1,
    eUIType_btnUpdateAll = 2,
    eUIType_btnLoadAll = 3,
    eUIType_btnSaveAll = 4,
    eUIType_btnLoadScene = 5,
    eUIType_btnSaveEID = 6,
    eUIType_btnLoadEID = 7,
    eUIType_btnCameraFocus = 8,
    eUIType_btnNewItem = 9,
    eUIType_btnDeleteItem = 10,
    eUIType_btnCameraControl = 11,
};

#include "common/common.h"
#include "generated_config_struct_enum.h"
#include "vulkan.h"
#include "game_encode.h"
#include "game_asset.h"
#include "objectmanager.h"
#include "component.h"
#include "object.h"
#include "scene.h"
#include "transform.h"
#include "inputcontrol.h"
#include "model.h"
#include "plane.h"
#include "line.h"
#include "animation.h"
#include "postprocessing.h"
#include "cubemap.h"
#include "particle.h"
#include "material.h"
#include "light.h"
#include "axis.h"
#include "grid.h"
#include "ui.h"
#include "camera.h"
#include "graphics.h"
#include "engine.h"
#include "global.h"
#include "scene.h"
#include "command.h"
#include "rendersetting.h"
