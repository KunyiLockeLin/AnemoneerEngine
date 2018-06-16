#pragma once
#include "qeheader.h"

class QeCube : public QeModel {

public:
	QeCube(QeObjectMangerKey& _key) :QeModel(_key, eModel_Cube) {}
	//~QeCube() {}

	virtual void init(QeAssetXML* _property);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
	virtual void updateShowByCulling();
};