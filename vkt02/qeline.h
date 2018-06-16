#pragma once
#include "qeheader.h"

class QeLine : public QeModel
{
public:

	QeLine(QeObjectMangerKey& _key) :QeModel(_key, eModel_Line) {}
	
	std::string lineType;
	virtual void init(QeAssetXML* _property);
	virtual QeDataDescriptorSetModel createDescriptorSetModel(int index);
	virtual void createPipeline();
	virtual void setMatModel();
	virtual void updateCompute(float time);
	virtual void updateRender(float time);
	virtual void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};

