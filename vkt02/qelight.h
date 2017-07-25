#pragma once
#include "qeheader.h"

enum QeLightType {
	eLightPoint,
	eLightDirection,
	eLightSPot,
};


class QeLight
{
public:
	QeLight(QeObjectMangerKey& _key) {}
	~QeLight() {}

	QeVector3f pos;
	QeVector3f color;
	QeLightType type;
	float intensity;
	float radius;

	void update(float time) {}

};

