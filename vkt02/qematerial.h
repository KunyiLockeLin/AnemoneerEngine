#pragma once
#include "qeheader.h"

class QeMaterial : public QeComponent
{
public:
	QeMaterial(QeObjectMangerKey& _key) :QeComponent(_key) {}
	~QeMaterial() {}

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);

	bool bAlpha;
	QeDataMaterial materialData;
	QeAssetImage imageData;
	QeAssetGraphicsShader shaderData;
};