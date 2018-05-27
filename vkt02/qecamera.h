#pragma once

#include "qeheader.h"

//enum QeCameraType {
//	eCameraThirdPerson,
//	eCameraFirstPerson,
//};

class QeCamera : public QeBase
{
public:
	QeVector3f pos;
	QeVector3f target;
	QeVector3f up;
	QeVector2i lastMousePos;
	QeMatrix4x4f view;
	QeMatrix4x4f projection;
	QeVector3f face;

	float fov = 45.f;
	float fnear = 0.1f;
	float ffar = 1000.f;
	float faspect = 1.f;
	float speed = 0.5f;
	int cullingDistance = 0;
	//QeCameraType type = eCameraThirdPerson;
	QeAssetXML* initProperty = nullptr;

	QeCamera(QeObjectMangerKey& _key):QeBase() {}
	~QeCamera() {}
	void init(QeAssetXML* _property);
	void reset();
	//void switchType(QeCameraType _type);
	void setCamera(QeVector3f& _pos, QeVector3f& _target, QeVector3f& _up, float _fov, float _near, float _far );
	//void rotatePos(float _angle, QeVector3f _axis);
	void rotateTarget(float _angle, QeVector3f _axis);
	//void rotatePos(QeVector2i mousePos);
	void rotateTarget(QeVector2i mousePos);
	void setMousePos(QeVector2i mousePos);
	void zoomInOut(QeVector2i mousePos);
	void move(QeVector3f _dir, bool bMoveTarget = true);
	void updateRender(float time);
	void updateCompute(float time);
	void updateAxis();
};

