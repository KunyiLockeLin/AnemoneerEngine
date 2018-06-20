#pragma once
#include "qeheader.h"

class QeCubemap : public QeModel {

public:
	QeCubemap(QeObjectMangerKey& _key, QeObjectType _type = eObject_Cubemap) :QeModel(_key, _type) {}
	//~QeCubemap() {}

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void updateShowByCulling();
};