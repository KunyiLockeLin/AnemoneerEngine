#pragma once

#include "qeheader.h"

class QeActivity {

public:
	QeActivity(QeQueenEngineKey& _key) {}
	//~QeActivity() {}

	int eid;
	QeVector4f	ambientColor;
	QeAssetXML* initProperty = nullptr;
	QeLight * light = nullptr;
	QeLine * axis = nullptr;
	QeLine * grids = nullptr;

	void init(QeAssetXML * _property);
	void eventInput(QeInputData & inputData);
	void updateCompute(float time);
	void updateRender(float time);
};

