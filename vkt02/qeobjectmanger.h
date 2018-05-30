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

	std::map<uint16_t, QeBase*> mgrObjs;
	std::map<uint16_t, QeModel*> mgrModels;
	std::vector<QeModel*> mgrAlphaModels;
	/*
	const uint16_t ID_CAMERA_MIN			= 10000;
	const uint16_t ID_CAMERA_MAX			= 10999;
	const uint16_t ID_LIGHT_MIN				= 11000;
	const uint16_t ID_LIGHT_MAX				= 11999;
	const uint16_t ID_ACTIVITY_MIN			= 12000;
	const uint16_t ID_ACTIVITY_MAX			= 12999;
	const uint16_t ID_MODEL_MIN				= 30000;
	const uint16_t ID_MODEL_MAX				= UINT16_MAX;
	*/

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger();
	
	QeBase* getPoint(int _id, QeAssetXML* _property);
	QeCamera* getCamera(int _id, QeAssetXML* _property);
	QeLight* getLight(int _id, QeAssetXML* _property);
	//QeActivity* getActivity(int _id, QeAssetXML* _property);
	QeModel* getModel(int _id, QeAssetXML* _property);
	QeCube* getCube(int _id, QeAssetXML* _property);
	QeBillboard* getBillboard(int _id, QeAssetXML* _property);
	QeLine* getLine(int _id, QeAssetXML* _property, const char* lineType = nullptr);
	QeParticle* getParticle(int _id, QeAssetXML* _property);

	void sortAlphaModels();
	void addAlphaModels(QeModel* model);

	void updateRender(float _time);
	void updateCompute(float _time);

	void cleanupPipeline();
	void recreatePipeline();

	void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	void updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};

