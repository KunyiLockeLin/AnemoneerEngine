#pragma once
#include "qeheader.h"

enum QeLightType {
	eLightPoint,
	eLightDirection,
	eLightSpot,
};

struct QeDataLight {
	QeVector4f pos;
	QeVector4f dir;
	QeVector4f color;
	QeVector4f param; // 1: type, 2: intensity, 3: coneAngle
};

class QeLight:public QePoint
{
public:
	QeLight(QeObjectMangerKey& _key, QeObjectType _type = eObject_Light) :QePoint(_key, _type) {}
	//~QeLight() {}

	float speed;
	QeVector3f center;
	QeVector3f up;

	QeDataLight bufferData;
	bool bUpdate;

	virtual void setProperty();
	virtual void updateCompute(float time);
	virtual void updateRender(float time);
};

