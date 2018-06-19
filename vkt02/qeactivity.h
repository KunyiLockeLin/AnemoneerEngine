#pragma once

#include "qeheader.h"

class QeActivity
{
public:
	QeActivity(QeQueenEngineKey& _key) {}
	~QeActivity() {}

	int id;
	std::string name;
	QeVector4f	ambientColor;
	QeAssetXML* initProperty = nullptr;
	QeLight * light = nullptr;
	QeModel * axis = nullptr;
	QeModel * grids = nullptr;

	void init(QeAssetXML * _property);
	void eventInput(QeInputData & inputData);
	void updateRender(float time);
	void updateCompute(float time);
};

