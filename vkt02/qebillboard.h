#pragma once
#include "qeheader.h"

class QeBillboard : public QeModel
{
public:
	// face: (0,0,1)

	QeBillboard(QeObjectMangerKey& _key) :QeModel(_key) {}
	//~QeBillboard() {}

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
};