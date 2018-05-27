#pragma once

#define NOMINMAX

#include <windows.h>
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
#include <cmath>
#include <chrono>
#include <ctime>
#include <forward_list>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <unordered_map>

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
#define KEY_L 0x4C
#define KEY_X 0x58
#define KEY_Z 0x5A

//	VkPhysicalDeviceLimits::maxViewports  VK->deviceProperties.limits.maxViewports;
const uint8_t MAX_VIEWPORT_NUM = 9;
const uint8_t MAX_JOINT_NUM = 20;
const char BONE_ROOT_NAME[] = "Armature_root";
const uint16_t MAX_DESCRIPTOR_UNIFORM_NUM = 512;
const uint16_t MAX_DESCRIPTOR_SAMPLER_NUM = 512;
const uint16_t MAX_DESCRIPTOR_INPUTATTACH_NUM = 64;
const uint16_t MAX_DESCRIPTOR_STORAGETEXEL_NUM = 512;
const uint16_t MAX_DESCRIPTOR_NUM = 128;
const uint16_t MAX_PARTICLES_NUM = UINT16_MAX;
const int PUSH_CONSTANTS_SIZE = 4;
const int GRIDS_SIZE = 10;
const int AXIS_SIZE = 1;

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
struct QeDataMaterial;
struct QeDataMaterialPhong;
struct QeDataMaterialPBR;
struct QeAssetImage;
struct QeAssetShader;
struct QeUniformBufferObject;
struct QeDataLight;
struct QeHuffmanTree;
struct QeAssetXML;
struct QeAssetJSON;
struct QeSkeleton;
struct QeInputData;
class QeBase;
class QeEncode;
class QeMath;
class QeAsset;
class QeModel;
class QeBillboard;
class QeCube;
class QeParticle;
class QeCompute;
class QeCamera;
class QeLight;
class QeGlobalKey;
class QeGlobal;
class QeWindow;
class QeViewport;
class QueenEngine;
class QeObjectMangerKey;
class QeObjectManger;
class QeActivity;
class QeVulkan;
class QeLog;
class QeCommand;
class QeTimer;
class QeLine;

#include "qeencode.h"
#include "qemath.h"
#include "qetimer.h"
#include "qevulkan.h"
#include "qeasset.h"
#include "qebase.h"
#include "qemodel.h"
#include "qebillboard.h"
#include "qeline.h"
#include "qecube.h"
#include "qeparticle.h"
#include "qecompute.h"
#include "qelight.h"
#include "qewindow.h"
#include "qeviewport.h"
#include "qecamera.h"
#include "queenengine.h"
#include "qeglobal.h"
#include "qeobjectmanger.h"
#include "qeactivity.h"
#include "qelog.h"
#include "qecommand.h"

using namespace QeString;