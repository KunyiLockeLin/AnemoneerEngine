#pragma once
#include "qeheader.h"


class QeLine : public QeModel
{
public:

	QeLine(QeObjectMangerKey& _key, QeObjectType _type = eObject_Line ):QeModel(_key, _type) {}
	
	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
	virtual void setMatModel();
	virtual void updateCompute(float time);
	virtual void updateRender(float time);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};

