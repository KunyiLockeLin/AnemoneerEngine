#pragma once

#include "qeheader.h"

class QeCamera
{
private:
	QeVector3f pos;
	QeVector3f target;
	QeVector3f up;
	QeVector2i lastMousePos;
	float fov;
	float fnear;
	float ffar;
	float rotateSpeed = 0.5f;
	float moveSpeed = 1.0f;
	bool bFirstPerson;

public:
	QeCamera() {}
	~QeCamera() {}
	void setFirstPerson(bool _bFirstPerson);
	void setCamera(QeVector3f _pos, QeVector3f _target, QeVector3f _up, float _fov, float _near, float _far );
	void rotatePos(float _angle, QeVector3f _axis);
	void rotateTarget(float _angle, QeVector3f _axis);
	void rotatePos(QeVector2i mousePos);
	void rotateTarget(QeVector2i mousePos);
	void setMousePos(QeVector2i mousePos);
	void move(QeVector3f _dir);
	QeMatrix4x4f getMatView();
	QeMatrix4x4f getMatProjection();
	void update();
	void reset();
};

