#pragma once

#include "qeheader.h"

struct QeDataCamera {
	QeVector4f pos_rayTracingDepth;
	QeVector4f face_focusDist;
	QeVector4f up_aperture;
	QeVector4f fov_aspect_near_far;
	QeMatrix4x4f view;
	QeMatrix4x4f projection;
};

class QeCamera : public QeComponent
{
public:

	QeCamera(QeObjectMangerKey& _key):QeComponent(_key){}
	~QeCamera() {}

	QeRenderType renderType;
	int lookAtTransformOID;
	VkExtent2D renderSize;

	float speed;
	int cullingDistance;
	QeVector2i lastMousePos;
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