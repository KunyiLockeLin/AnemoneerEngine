#include "qeobjectmanger.h"

QeCamera* QeObjectManger::getCamera(const char* _name) {

	std::map<std::string, QeCamera*>::iterator it = mgrCameras.find(_name);
	if (it != mgrCameras.end())	return it->second;

	QeCamera* newCamera = new QeCamera(key);
	newCamera->init();
	mgrCameras[_name] = newCamera;

	return newCamera;
}

QeLight* QeObjectManger::getLight(const char* _name) {
	std::map<std::string, QeLight*>::iterator it = mgrLights.find(_name);
	if (it != mgrLights.end())	return it->second;

	QeLight* newLight = new QeLight(key);
	newLight->init();
	mgrLights[_name] = newLight;

	return newLight;
}

QeActivity* QeObjectManger::getActivity(const char* _name) {
	std::map<std::string, QeActivity*>::iterator it = mgrActivitys.find(_name);
	if (it != mgrActivitys.end())	return it->second;

	QeActivity* newActivity = new QeActivity(key);
	newActivity->init();
	mgrActivitys[_name] = newActivity;

	return newActivity;
}

QeModel* QeObjectManger::getModel(const char* _name, int _index ) {

	QeModel* newModel = nullptr;
	if (!mgrInactiveModels.empty()) {
		newModel = mgrInactiveModels.back();
		mgrInactiveModels.pop_back();
	}
	else
		newModel = new QeModel(key);

	newModel->init(AST->getString("model", _index) );
	mgrActiveModels.push_back(newModel);

	VLK->updateDrawCommandBuffers(mgrActiveModels);
	return newModel;
}

void QeObjectManger::update(float _time) {

	std::map<std::string, QeCamera*>::iterator it = mgrCameras.begin();
	while (it != mgrCameras.end()) {
		it->second->update(_time);
		++it;
	}

	std::map<std::string, QeLight*>::iterator it1 = mgrLights.begin();
	while (it1 != mgrLights.end()) {
		it1->second->update(_time);
		++it1;
	}

	std::vector<QeModel*>::iterator it2 = mgrActiveModels.begin();
	while (it2 != mgrActiveModels.end()) {
		(*it2)->update(_time);
		++it2;
	}
}


void QeObjectManger::cleanupSwapChain() {

	std::vector<QeModel*>::iterator it2 = mgrActiveModels.begin();
	while (it2 != mgrActiveModels.end()) {
		(*it2)->cleanupSwapChain();
		++it2;
	}
}

void QeObjectManger::recreateSwapChain() {

	std::vector<QeModel*>::iterator it2 = mgrActiveModels.begin();
	while (it2 != mgrActiveModels.end()) {
		(*it2)->recreateSwapChain();
		++it2;
	}

	VLK->updateDrawCommandBuffers(mgrActiveModels);
}