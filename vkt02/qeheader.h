#pragma once

#define NOMINMAX

#include <windows.h>
#include <tchar.h> 
#include <cstdlib> 
#include <string>  
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <unordered_map>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_W 0x57

//	VkPhysicalDeviceLimits::maxViewports  VK->deviceProperties.limits.maxViewports;
const int MAX_VIEWPORT_NUM = 9;
const int MAX_JOINT_NUM = 20;
const char BONE_ROOT_NAME[] = "Armature_root";

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
struct QeDataPBRMaterial;
struct QeAssetImage;
struct QeAssetShader;
struct QeUniformBufferObject;
struct QeDataLight;
struct QeHuffmanTree;
struct QeAssetXML;
struct QeAssetJSON;
struct QeSkeleton;
class QeEncode;
class QeMath;
class QeAsset;
class QeModel;
class QeBillboard;
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

#include "qeencode.h"
#include "qemath.h"
#include "qevulkan.h"
#include "qeasset.h"
#include "qemodel.h"
#include "qebillboard.h"
#include "qecamera.h"
#include "qelight.h"
#include "qewindow.h"
#include "qeviewport.h"
#include "queenengine.h"
#include "qeglobal.h"
#include "qeobjectmanger.h"
#include "qeactivity.h"