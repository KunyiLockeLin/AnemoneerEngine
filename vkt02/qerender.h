#pragma once
#include "qeheader.h"

class QeRender : public QeModel
{
public:
	QeRender(QeObjectMangerKey& _key) :QeModel(_key) {}
	//~QeCubemap() {}

	VkExtent2D renderSize;
	int targetCameraOID;
	bool bUpdate;

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void update1();
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
};

