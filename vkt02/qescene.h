#pragma once

#include "qeheader.h"

class QeScene {

public:
	QeScene(QeGlobalKey& _key) {}
	~QeScene() {}


	int eid;
	QeAssetXML* initProperty = nullptr;
	std::vector<QeObject*> objects;


	void initialize(QeAssetXML * _property);
	void clear();
	void update1();
	void update2();
};
