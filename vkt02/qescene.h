#pragma once

#include "qeheader.h"

class QeScene {

public:
	QeScene(QeQueenEngineKey& _key) {}
	//~QeQeScene() {}

	int eid;
	QeVector4f	ambientColor;
	QeAssetXML* initProperty = nullptr;
	QeLight * light = nullptr;
	QeLine * axis = nullptr;
	QeLine * grids = nullptr;
	int gridsNum;

	void init(QeAssetXML * _property);
	void eventInput(QeInputData & inputData);
	void update1() {}
	void update2() {}
};

