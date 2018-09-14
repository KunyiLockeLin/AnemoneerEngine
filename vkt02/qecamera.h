#pragma once

#include "qeheader.h"

//enum QeCameraType {
//	eCameraThirdPerson,
//	eCameraFirstPerson,
//};

struct QeDataCamera {
	QeVector4f pos;
	QeMatrix4x4f view;
	QeMatrix4x4f projection;
};

class QeCamera : public QePoint
{
public:
	
	QeCamera(QeObjectMangerKey& _key, QeObjectType _type = eObject_Camera) :QePoint(_key, _type) {}
	//~QeCamera() {}

	QeVector3f center;
	QeVector3f up;
	QeVector2i lastMousePos;
	QeVector3f face;

	bool bUpdate;
	QeDataCamera bufferData;
	//float polarAngle, azimuthalAngle;

	float fov;
	float fnear;
	float ffar;
	float faspect;
	float speed;
	int cullingDistance;
	//QeCameraType type = eCameraThirdPerson;

	virtual void setProperty();
	virtual void reset();
	void setMatrix();
	//void switchType(QeCameraType _type);
	//void rotatePos(float _angle, QeVector3f _axis);
	void rotateTarget(float _angle, QeVector2f _axis);
	//void rotatePos(QeVector2i mousePos);
	void rotateTarget(QeVector2i mousePos);
	void setMousePos(QeVector2i mousePos);
	void zoomInOut(QeVector2i mousePos);
	void move(QeVector3f _dir, bool bMoveCenter = true);
	virtual void update1();
	void updateAxis();
};