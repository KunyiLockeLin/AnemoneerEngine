#pragma once

#define NOMINMAX

#include <windows.h>
#include <process.h>
#include "dbghelp.h"
#include <tchar.h>
#include <cstdlib>
#include <string>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <thread>
#include <memory>
#include <commctrl.h>
#include <cmath>
#include <chrono>
#include <ctime>
#include <forward_list>
#include <utility>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <unordered_map>
#include <direct.h>

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
const char INDEX_NONE = -1;
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
struct QeVector2i;
struct QeVector2f;
struct QeVector3i;
struct QeVector3f;
struct QeVector4f;
struct QeMatrix4x4f;
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
struct QeHuffmanTree;
struct QeAssetXML;
struct QeAssetJSON;
struct QeSkeleton;
struct QeInputData;
struct QeDataSwapchain;
struct QeDataDrawCommand;
class QeObject;
class QeComponent;
class QeTransform;
class QeInputControl;
class QePostProcessing;
class QeEncode;
class QeMath;
class QeAsset;
class QeModel;
class QePlane;
class QeCubemap;
class QeParticle;
class QeCompute;
class QeCamera;
class QeLight;
class AeGlobalKey;
class AeGlobal;
class QeWindow;
class QeViewport;
class AngryEngine;
class AeObjectManagerKey;
class AeObjectManager;
class QeVulkan;
class QeLog;
class QeCommand;
class QeTimer;
class QeLine;
class QeAxis;
class QeGrid;
class QeRenderSetting;

enum QeActionType {
    eActionTypeOnce = 0,
    eActionTypeReplay = 1,
    eActionTypeNext = 2,
};

enum QeActionState {
    eActionStateStop = 0,
    eActionStatePlay = 1,
    eActionStatePause = 2,
};

enum QeModelDataType {
    eModelData_cube = 0,
    eModelData_axis = 1,
    eModelData_grid = 2,
    eModelData_line = 3,
    eModelData_plane = 4,
    eModelData_gltf = 5,
};

enum QeAssetType {
    eAssetModel = 0,
    eAssetMaterial = 1,
    eAssetBin = 2,
    eAssetShader = 3,
    eAssetTexture = 4,
    eAssetXML = 5,
    eAssetJSON = 6,
};

enum QeComponentType {
    eComponent_transform = 1,
    eComponent_camera = 2,
    eComponent_postprocessing = 3,
    eComponent_light = 4,
    eComponent_line = 5,
    eComponent_model = 6,
    eComponent_animation = 7,
    eComponent_plane = 8,
    eComponent_cubemap = 9,
    eComponent_partical = 10,
    eComponent_material = 11,
    eComponent_inputControl = 12,
    eComponent_axis = 13,
    eComponent_grid = 14,
    eComponent_rendersetting = 15,
    eObject = 1000,
    eScene = 2000,
};

enum QeRenderType { eRender_KHR = 0, eRender_ui = 1, eRender_main = 2, eRender_color = 3, eRender_MAX = 4 };

enum QeLightType {
    eLightPoint = 0,
    eLightDirection = 1,
    eLightSpot = 2,
};

enum QePlaneType {
    ePlane_3D = 0,
    ePlane_billboard = 1,
    ePlane_2D = 2,
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

#include "encode.h"
#include "math.h"
#include "timer.h"
#include "vulkan.h"
#include "asset.h"
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
#include "window.h"
#include "camera.h"
#include "graphics.h"
#include "engine.h"
#include "global.h"
#include "scene.h"
#include "log.h"
#include "command.h"
#include "rendersetting.h"

using namespace QeLib;
