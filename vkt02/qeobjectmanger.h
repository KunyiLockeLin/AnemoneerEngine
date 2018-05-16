#pragma once

#include "qeheader.h"

class QeObjectMangerKey
{
	friend class QeObjectManger;

private:
	QeObjectMangerKey() {}
};


class QeObjectManger
{
private:
	QeObjectMangerKey key;
public:

	std::map<int, QeCamera*> mgrCameras;
	std::map<int, QeLight*> mgrLights;
	std::map<int, QeActivity*> mgrActivitys;
	std::map<int, QeModel*> mgrModels;
	std::map<int, QeModel*> mgrBillboards;

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger();
	
	QeCamera* getCamera(int _id, QeAssetXML* _property);
	QeLight* getLight(int _id, QeAssetXML* _property);
	QeActivity* getActivity(int _id, QeAssetXML* _property);
	QeModel* getModel(int _id, QeAssetXML* _property);
	QeBillboard* getBillboard(int _id);
	
	void updateRender(float _time);
	void updateCompute(float _time);

	void cleanupPipeline();
	void recreatePipeline();

	void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};

