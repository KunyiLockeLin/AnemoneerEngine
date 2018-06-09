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

class QeLight:public QeBase
{
public:
	QeLight(QeObjectMangerKey& _key):QeBase(_key), uboBuffer(eBuffer_uniform) {}
	~QeLight() {}

	bool bShow = false;
	int speed = 0;
	QeVector3f rotateCenter;
	QeDataLight data;
	QeVKBuffer uboBuffer;

	void init(QeAssetXML* _property);
	void updateRender(float time);
	void updateCompute(float time);

	QeBillboard* billboard = nullptr;
};

