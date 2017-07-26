#pragma once
#include "qeheader.h"

enum QeLightType {
	eLightPoint,
	eLightDirection,
	eLightSPot,
};

struct QeDataLight {
	QeVector3f pos;
	QeVector3f dir;
	QeVector3f color;
	QeLightType type;
	float intensity;
	float radius;
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

