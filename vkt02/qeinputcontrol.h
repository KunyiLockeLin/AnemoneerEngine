#pragma once
#include "qecomponent.h"


class QeInputControl : public QeComponent
{
public:
	QeInputControl(QeObjectMangerKey& _key) :QeComponent(_key) {}
	~QeInputControl() {}

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void clear();

	virtual void updateInput();
};

