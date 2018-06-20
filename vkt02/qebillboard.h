#pragma once
#include "qeheader.h"


enum QeBillboardType {
	eBillboardFace,
	eBillboardFaceAndSize,
};


class QeBillboard : public QeModel
{
public:
	// face: (0,0,1)

	QeBillboard(QeObjectMangerKey& _key, QeObjectType _type = eObject_Billboard ): QeModel(_key, _type){}
	//~QeBillboard() {}

	QeBillboardType type = eBillboardFace;

	virtual void init(QeAssetXML* _property, int _parentOID);

	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);

	virtual void createPipeline();
	virtual void setMatModel();

	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};