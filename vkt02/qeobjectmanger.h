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
	std::map<std::string, QeLight*> mgrLights;
	std::map<std::string, QeActivity*> mgrActivitys;
	std::vector<QeModel*> mgrInactiveModels;
	std::vector<QeModel*> mgrActiveModels;

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger() {}
	
	QeCamera* getCamera(int _id);
	QeLight* getLight(const char* _name = "MAIN");
	QeActivity* getActivity(const char* _name = "MAIN");
	QeModel* getModel(const char* _name, int _index=0);

	void update(float _time);

	void cleanupSwapChain();
	void recreateSwapChain();
};

