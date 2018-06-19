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
	QeLight(QeObjectMangerKey& _key):QeBase(_key) {}
	~QeLight() {}

	bool bShow = false;
	float speed = 0.0f;
	QeVector3f rotateCenter;

	QeDataLight bufferData;
	//VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	bool bUpdateBuffer = true;

	void init(QeAssetXML* _property);
	void updateRender(float time);
	void updateCompute(float time);

	QeBillboard* billboard = nullptr;
};

