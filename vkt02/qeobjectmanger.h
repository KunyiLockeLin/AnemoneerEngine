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
	std::vector<QeModel*> mgrInactiveModels;
	std::vector<QeModel*> mgrActiveModels;
	std::vector<QeModel*> mgrInactiveBillboards;
	std::vector<QeModel*> mgrActiveBillboards;

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger() {}
	
	QeCamera* getCamera(int _id);
	QeLight* getLight(int _id);
	QeActivity* getActivity(int _id);
	QeModel* getModel(const char* _name);
	QeBillboard* getBillboard(int _id);
	
	void update(float _time);

	void cleanupSwapChain();
	void recreateSwapChain();

	std::vector<QeModel*> getDrawObject();
};

