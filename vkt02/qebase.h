#pragma once

#include "qeheader.h"

class QeBase
{
public:
	int id=0;
	QeAssetXML* initProperty = nullptr;
	QeVector3f pos;
	int particleID = 0;
	QeParticle* particle;

	QeBase(QeObjectMangerKey& _key) {}
	virtual void init(QeAssetXML* _property);
	virtual void updateRender(float time);
	virtual void updateCompute(float time);
};