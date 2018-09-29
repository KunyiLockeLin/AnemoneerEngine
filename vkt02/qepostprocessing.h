#pragma once
#include "qeheader.h"


class QePostProcessing : public QeComponent
{
public:
	QePostProcessing(QeObjectMangerKey& _key) :QeComponent(_key) {}
	//~QePostProcessing();

	QeVector4f param1;
	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
};

