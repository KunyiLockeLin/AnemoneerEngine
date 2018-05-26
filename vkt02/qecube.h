#pragma once
#include "qeheader.h"

class QeCube : public QeModel {

public:
	QeCube(QeObjectMangerKey& _key) :QeModel(_key) {}
	//~QeCube() {}

	virtual void init(QeAssetXML* _property);
	virtual void updateShowByCulling();
};