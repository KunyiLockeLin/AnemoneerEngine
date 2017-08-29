#pragma once
// Right-handed Coordinate System
#include "qeheader.h"

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


class QeMath
{
public:
	QeMath(QeGlobalKey& _key) {}
	~QeMath() {}

	const float PI = 3.141592654f;
	const float RADIANS_TO_DEGREES = 180.0f / PI;
	const float DEGREES_TO_RADIANS = PI / 180;

	QeMatrix4x4f lookAt(QeVector3f _pos, QeVector3f _target, QeVector3f _up);
	QeMatrix4x4f perspective(float _fov, float _aspect, float _near, float _far);
	QeMatrix4x4f translate(QeVector3f _pos);
	QeMatrix4x4f rotate(float _angle, QeVector3f _axis);
	QeMatrix4x4f rotateX(float _angle);
	QeMatrix4x4f rotateY(float _angle);
	QeMatrix4x4f rotateZ(float _angle);
	QeMatrix4x4f scale(QeVector3f _size);
	QeVector3f normalize(QeVector3f _vec);
	float length(QeVector3f _vec);
	float distance(QeVector3f _from, QeVector3f _to);
	float dot(QeVector3f _vec1, QeVector3f _vec2);
	QeVector3f cross(QeVector3f _vec1, QeVector3f _vec2);
	float fastSqrt(float _number);
	bool inverse(QeMatrix4x4f _inMat, QeMatrix4x4f& _outMat);
	QeMatrix4x4f transpose(QeMatrix4x4f _mat);
};