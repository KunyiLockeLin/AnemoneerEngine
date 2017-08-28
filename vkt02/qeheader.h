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
#include <array>
#include <set>
#include <map>
#include <unordered_map>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_W 0x57

const int MAX_VIEWPORT_NUM = 9;

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
struct QeAssetImage;
struct QeAssetShader;
struct QeDataMVP;
struct QeDataLight;
class QeMath;
class QeAsset;
class QeModel;
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


struct QeVector2i {
	int x, y;

	QeVector2i();
	QeVector2i(int _x, int _y);
};

struct QeVector2f {
	float x, y;

	QeVector2f();
	QeVector2f(float _x, float _y);

	bool operator==(const QeVector2f& other)const;
	QeVector2f& operator+=(const QeVector2f& other);
	QeVector2f& operator-=(const QeVector2f& other);
	QeVector2f& operator/=(const float& other);
	QeVector2f operator/(const float& other);
	QeVector2f operator*(const float& other);
};

struct QeVector3i {
	int x, y, z;

	QeVector3i();
	QeVector3i(int _x, int _y, int _z);

	bool operator==(const QeVector3i& other)const;
	QeVector3i& operator+=(const QeVector3i& other);
	QeVector3i& operator-=(const QeVector3i& other);
	QeVector3i& operator-=(const int& other);
	QeVector3i& operator/=(const int& other);
	QeVector3i operator-(const QeVector3i& other);
	QeVector3i operator/(const int& other);
	QeVector3i operator*(const int& other);
};


struct QeVector3f {
	float x, y, z;

	QeVector3f();
	QeVector3f(float _x, float _y, float _z);

	bool operator==(const QeVector3f& other)const;
	QeVector3f& operator=(const QeVector4f& other);
	QeVector3f& operator+=(const QeVector3f& other);
	QeVector3f& operator-=(const QeVector3f& other);
	QeVector3f& operator-=(const float& other);
	QeVector3f& operator/=(const float& other);
	QeVector3f& operator*=(const float& other);
	QeVector3f operator+(const QeVector3f& other);
	QeVector3f operator-(const QeVector3f& other);
	QeVector3f operator/(const float& other);
	QeVector3f operator*(const float& other);
};

struct QeVector4f {
	float x, y, z, w;

	QeVector4f();
	QeVector4f(float _x, float _y, float _z, float _w);
	QeVector4f(const QeVector3f& other, float _w);

	QeVector4f& operator=(const QeVector3f& other);
};

struct QeMatrix4x4f {
	float _00, _01, _02, _03;
	float _10, _11, _12, _13;
	float _20, _21, _22, _23;
	float _30, _31, _32, _33;

	QeMatrix4x4f();
	QeMatrix4x4f(float _num);
	QeMatrix4x4f(float __00, float __01, float __02, float __03, float __10, float __11, float __12, float __13, float __20, float __21, float __22, float __23, float __30, float __31, float __32, float __33);
	QeMatrix4x4f& operator*=(const QeMatrix4x4f& other);
	QeMatrix4x4f operator*(const QeMatrix4x4f& other);
	QeVector4f operator*(const QeVector4f& other);
	QeMatrix4x4f& operator/=(const float& other);
};


#include "qemath.h"
#include "qeasset.h"
#include "qevulkan.h"
#include "qemodel.h"
#include "qecamera.h"
#include "qelight.h"
#include "qewindow.h"
#include "qeviewport.h"
#include "queenengine.h"
#include "qeglobal.h"
#include "qeobjectmanger.h"
#include "qeactivity.h"