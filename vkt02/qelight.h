#pragma once
#include "qeheader.h"

class QeLight
{
public:
	QeLight();
	~QeLight();

	QeVector3f pos;
	QeVector3f color;
	float intensity;

	void update() {}

};

