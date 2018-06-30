#pragma once

#include "qeheader.h"

/* 10,000, - 32,767
eid: 10,000 - 30,000
line:		10,000 - 10,999
billboard:	11,000 - 11,999
cubemap:	12,000 - 12,999
particle:	13,000 - 13,999
model:		14,000 - 14,999

scene:		28,000 - 28,999


oid:  10,000 - 32,767
line:		10,000 - 10,999
billboard:	11,000 - 11,999
cubemap:	12,000 - 12,999
particle:	13,000 - 13,999
model:		14,000 - 14,999

point:		25,000 - 25,999
camera:		26,000 - 26,999
light:		27,000 - 27,999

*/

enum QeObjectType {
	eObject_Point = 25,
	eObject_Camera = 26,
	eObject_Light = 27,
	eObject_Scene = 28,

	eObject_Line = 10,
	eObject_Billboard = 11,
	eObject_Cubemap = 12,
	eObject_Particle = 13,
	eObject_Model = 14,
	eObject_Render = 15,

};

class QeObjectMangerKey {
	friend class QeObjectManger;

private:
	QeObjectMangerKey() {}
};

class QeObjectManger {

private:
	QeObjectMangerKey key;

	std::map<int, QePoint*> mgrObjs;
	std::map<int, QeModel*> mgrModels;
	std::vector<QeModel*> mgrAlphaModels;

public:

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger();	

	QePoint* getObject(int _oid, QeAssetXML* _property=nullptr, int _parentOID=0);

	void sortAlphaModels(QeCamera* camera);

	void updateCompute(float _time);
	void updateRender(float _time);

	void cleanupPipeline();
	void recreatePipeline();

	void updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet);
	void updateDrawCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet, VkRenderPass& renderPass);
};