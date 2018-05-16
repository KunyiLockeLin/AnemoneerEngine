#pragma once

#include "qeheader.h"

class QeActivity
{
public:
	QeActivity(QeObjectMangerKey& _key) {}
	~QeActivity() {}

	const char* name = nullptr;
	QeVector4f	ambientColor;
	QeAssetXML* initProperty = nullptr;

	void init(QeAssetXML * _property);
	void eventInput(QeInputData & inputData);
	void update(float time);
};

