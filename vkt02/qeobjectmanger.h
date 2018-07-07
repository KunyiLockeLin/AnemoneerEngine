#pragma once

#include "qeheader.h"


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
	void updateDrawCommandBuffer(QeDataDrawCommand* command );
};