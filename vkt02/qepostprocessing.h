#pragma once
#include "qeheader.h"


class QePostProcessing : public QeComponent
{
public:
	QePostProcessing(QeObjectMangerKey& _key) :QeComponent(_key) {}
	//~QePostProcessing();

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
};

