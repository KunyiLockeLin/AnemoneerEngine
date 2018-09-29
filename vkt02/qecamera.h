#pragma once

#include "qeheader.h"

struct QeDataCamera {
	QeVector4f pos;
	QeMatrix4x4f view;
	QeMatrix4x4f projection;
};

class QeCamera : public QeComponent
{
public:

	QeCamera(QeObjectMangerKey& _key):QeComponent(_key){}
	~QeCamera() {}

	QeRenderType cameraType;
	int lookAtTransformOID;
	VkExtent2D renderSize;

	QeVector3f up;
	float fov;
	float fnear;
	float ffar;
	float speed;
	int cullingDistance;
	bool b2D;

	QeVector2i lastMousePos;
	float faspect;
	int postProcessingOID;
	bool bUpdatePostProcessingOID;
	QeDataCamera bufferData;

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);

	virtual void update1();

	virtual void reset();
	void setLookAtTransformOID( int _lookAtTransformOID);
	void rotateTarget(QeVector3f _addRotate);
	void rotateTargetByMouse(QeVector2i mousePos);
	void setMousePos(QeVector2i mousePos);
	void zoomInOut(QeVector2i mousePos);
	void move(QeVector3f _dir, bool bMoveCenter = true);
	QeVector3f face();
	QeVector3f lookAt();
};