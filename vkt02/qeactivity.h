#pragma once

#include "qeheader.h"

class QeActivity
{
public:
	QeActivity(QeObjectMangerKey& _key) {}
	~QeActivity() {}

	const char* name;
	QeVector4f	ambientColor;
	QeAssetXML* initProperty;

	void init(QeAssetXML* _property);
	void eventInput( int _input1, int _input2, int _param1, int _param2);
	void update(float time);
};

