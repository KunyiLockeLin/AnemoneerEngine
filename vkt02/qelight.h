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

class QeLight
{
public:
	QeLight(QeObjectMangerKey& _key) {}
	~QeLight() {}
	int speed = 0;
	QeVector3f rotateCenter;
	QeDataLight data;
	void init();
	void setProperty(QeAssetXML* _property);
	void update(float time);

	QeBillboard* billboard;
};

