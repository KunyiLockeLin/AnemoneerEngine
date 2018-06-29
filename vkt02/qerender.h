#pragma once
#include "qeheader.h"

class QeRender : public QeModel
{
public:
	QeRender(QeObjectMangerKey& _key, QeObjectType _type = eObject_Render) :QeModel(_key, _type) {}
	//~QeRender(){}

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
};

