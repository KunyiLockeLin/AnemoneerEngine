#pragma once
#include "qeheader.h"

class QeBillboard : public QeModel
{
public:
	// face: (0,0,1)

	QeBillboard(QeObjectMangerKey& _key, QeObjectType _type = eObject_Billboard ): QeModel(_key, _type){}
	//~QeBillboard() {}

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};