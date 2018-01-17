#pragma once

#include "qeheader.h"

enum QeCameraType {
	eCameraThirdPerson,
	eCameraFirstPerson,
};

class QeCamera
{
public:
	//size_t id;

	QeVector3f pos;
	QeVector3f target;
	QeVector3f up;
	QeVector2i lastMousePos;
	float fov;
	float fnear;
	float ffar;
	float faspect = 1;
	float speed = 0.5f;
	QeCameraType type;
	QeAssetXML* initProperty;

	QeCamera(QeObjectMangerKey& _key) {}
	~QeCamera() {}
	void init(QeAssetXML* _property);
	void reset();
	void switchType(QeCameraType _type);
	void setCamera(QeVector3f _pos, QeVector3f _target, QeVector3f _up, float _fov, float _near, float _far );
	void rotatePos(float _angle, QeVector3f _axis);
	void rotateTarget(float _angle, QeVector3f _axis);
	void rotatePos(QeVector2i mousePos);
	void rotateTarget(QeVector2i mousePos);
	void setMousePos(QeVector2i mousePos);
	void move(QeVector3f _dir);
	QeMatrix4x4f getMatView();
	QeMatrix4x4f getMatProjection();
	void update(float time);

};

