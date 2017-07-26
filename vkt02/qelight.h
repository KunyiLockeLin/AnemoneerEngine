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
	QeLightType type;
	float intensity;
	float coneAngle;
};

class QeLight
{
public:
	QeLight(QeObjectMangerKey& _key) {}
	~QeLight() {}

	QeDataLight data;
	void init();
	void update(float time) {}

};

