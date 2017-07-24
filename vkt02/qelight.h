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
	QeLight();
	~QeLight();

	QeVector3f pos;
	QeVector3f color;
	QeLightType type;
	float intensity;

	void update(float time) {}

};

