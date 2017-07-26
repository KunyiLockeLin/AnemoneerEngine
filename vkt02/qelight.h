#pragma once
#include "qeheader.h"

enum QeLightType {
	eLightPoint,
	eLightDirection,
	eLightSpot,
};

struct QeDataLight {
	QeVector3f pos;
	QeVector3f dir; // for direction & Spot
	QeVector3f color;
	QeLightType type;
	float intensity;
	float radius; // for direction & Spot
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

