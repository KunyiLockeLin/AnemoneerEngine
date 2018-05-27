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

	/*
	std::map<int, QeCamera*> mgrCameras;
	std::map<int, QeLight*> mgrLights;
	std::map<int, QeActivity*> mgrActivitys;
	std::map<int, QeModel*> mgrModels;
	std::map<int, QeModel*> mgrCubes;
	std::map<int, QeModel*> mgrBillboards;
	std::map<int, QeModel*> mgrLines;
	std::map<int, QeModel*> mgrParticles;
	*/

	std::map<int, QeBase*> mgrObjs;
	/*
	const uint16_t ID_CAMERA_MIN			= 10000;
	const uint16_t ID_CAMERA_MAX			= 10999;
	const uint16_t ID_LIGHT_MIN				= 11000;
	const uint16_t ID_LIGHT_MAX				= 11999;
	const uint16_t ID_LIGHT_Billboard_MIN	= 12000;
	const uint16_t ID_LIGHT_Billboard_MAX	= 12999;
	const uint16_t ID_ACTIVITY_MIN			= 13000;
	const uint16_t ID_ACTIVITY_MAX			= 13999;
	const uint16_t ID_MODEL_MIN				= 30000;
	const uint16_t ID_MODEL_MAX				= UINT16_MAX;
	*/

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger();
	
	QeCamera* getCamera(int _id, QeAssetXML* _property);
	QeLight* getLight(int _id, QeAssetXML* _property);
	QeActivity* getActivity(int _id, QeAssetXML* _property);
	QeModel* getModel(int _id, QeAssetXML* _property);
	QeCube* getCube(int _id, QeAssetXML* _property);
	QeBillboard* getBillboard(int _id, QeAssetXML* _property);
	QeLine* getLine(int _id, QeAssetXML* _property, const char* lineType = nullptr);
	QeParticle* getParticle(int _id, QeAssetXML* _property);

	void updateRender(float _time);
	void updateCompute(float _time);

	void cleanupPipeline();
	void recreatePipeline();

	void updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer);
	void updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer);
};

