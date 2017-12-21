#include "qeheader.h"

QeCamera* QeObjectManger::getCamera(int _id) {

	std::map<int, QeCamera*>::iterator it = mgrCameras.find(_id);
	if (it != mgrCameras.end())	return it->second;

	QeCamera* newCamera = new QeCamera(key);
	newCamera->init();
	mgrCameras[_id] = newCamera;

	return newCamera;
}

QeLight* QeObjectManger::getLight(int _id) {
	std::map<int, QeLight*>::iterator it = mgrLights.find(_id);
	if (it != mgrLights.end())	return it->second;

	QeLight* newLight = new QeLight(key);
	newLight->init();
	mgrLights[_id] = newLight;

	return newLight;
}

QeActivity* QeObjectManger::getActivity(int _id) {
	std::map<int, QeActivity*>::iterator it = mgrActivitys.find(_id);
	if (it != mgrActivitys.end())	return it->second;

	QeActivity* newActivity = new QeActivity(key);
	newActivity->init();
	mgrActivitys[_id] = newActivity;

	return newActivity;
}

QeModel* QeObjectManger::getModel(const char* _name ) {

	QeModel* newModel = nullptr;
	if (!mgrInactiveModels.empty()) {
		newModel = mgrInactiveModels.back();
		mgrInactiveModels.pop_back();
	}
	else
		newModel = new QeModel(key);

	newModel->init(_name);
	mgrActiveModels.push_back(newModel);

	VLK->updateDrawCommandBuffers();
	return newModel;
}

QeBillboard* QeObjectManger::getBillboard(int _id) {

	QeModel* newModel = nullptr;
	if (!mgrInactiveBillboards.empty()) {
		newModel = mgrInactiveBillboards.back();
		mgrInactiveBillboards.pop_back();
	}
	else
		newModel = new QeBillboard(key);

	QeAssetXML* node = AST->getXMLNode(3, AST->CONFIG, "initWorld", "billboards");
	newModel->init(node->nexts[0]->value.c_str());
	mgrActiveBillboards.push_back(newModel);

	VLK->updateDrawCommandBuffers();
	return (QeBillboard*)newModel;
}

void QeObjectManger::update(float _time) {

	std::map<int, QeCamera*>::iterator it = mgrCameras.begin();
	while (it != mgrCameras.end()) {
		it->second->update(_time);
		++it;
	}

	std::map<int, QeLight*>::iterator it1 = mgrLights.begin();
	while (it1 != mgrLights.end()) {
		it1->second->update(_time);
		++it1;
	}

	std::vector<QeModel*>::iterator it2 = mgrActiveModels.begin();
	while (it2 != mgrActiveModels.end()) {
		(*it2)->update(_time);
		++it2;
	}

	it2 = mgrActiveBillboards.begin();
	while (it2 != mgrActiveBillboards.end()) {
		(*it2)->update(_time);
		++it2;
	}
}


void QeObjectManger::cleanupSwapChain() {

	std::vector<QeModel*>::iterator it = mgrActiveModels.begin();
	while (it != mgrActiveModels.end()) {
		(*it)->cleanupSwapChain();
		++it;
	}

	it = mgrActiveBillboards.begin();
	while (it != mgrActiveBillboards.end()) {
		(*it)->cleanupSwapChain();
		++it;
	}
}

void QeObjectManger::recreateSwapChain() {

	std::vector<QeModel*>::iterator it = mgrActiveModels.begin();
	while (it != mgrActiveModels.end()) {
		(*it)->recreateSwapChain();
		++it;
	}
	
	it = mgrActiveBillboards.begin();
	while (it != mgrActiveBillboards.end()) {
		(*it)->recreateSwapChain();
		++it;
	}
	VLK->updateDrawCommandBuffers();
}

std::vector<QeModel*> QeObjectManger::getDrawObject() {

	std::vector<QeModel*> vec = mgrActiveModels;
	vec.insert(vec.end(),mgrActiveBillboards.begin(), mgrActiveBillboards.end());
	return vec;
}