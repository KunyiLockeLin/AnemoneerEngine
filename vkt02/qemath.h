#pragma once
// Right-handed Coordinate System
#include "qeheader.h"


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