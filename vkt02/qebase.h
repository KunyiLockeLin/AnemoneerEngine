#pragma once

#include "qeheader.h"

class QeBase
{
public:
	uint16_t id;
	QeAssetXML* initProperty = nullptr;
	QeVector3f pos;
	QeParticle* particle;

	QeBase(QeObjectMangerKey& _key) {}
	virtual void init(QeAssetXML* _property);
	virtual void updateRender(float time);
	virtual void updateCompute(float time);
};