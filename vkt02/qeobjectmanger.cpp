#include "qeheader.h"


QeObjectManger::~QeObjectManger() {
	std::map<int, QeCamera*>::iterator it = mgrCameras.begin();
	while (it != mgrCameras.end()) {
		if ((it->second) != nullptr) delete (it->second);
		++it;
	}
	mgrCameras.clear();

	std::map<int, QeLight*>::iterator it1 = mgrLights.begin();
	while (it1 != mgrLights.end()) {
		if ((it1->second) != nullptr) delete (it1->second);
		++it1;
	}
	mgrLights.clear();

	std::map<int, QeActivity*>::iterator it2 = mgrActivitys.begin();
	while (it2 != mgrActivitys.end()) {
		if ((it2->second) != nullptr) delete (it2->second);
		++it2;
	}
	mgrActivitys.clear();

	std::map<int, QeModel*>::iterator it3 = mgrModels.begin();
	while (it3 != mgrModels.end()) {
		if (it3->second != nullptr) delete (it3->second);
		++it3;
	}
	mgrModels.clear();

	it3 = mgrBillboards.begin();
	while (it3 != mgrBillboards.end()) {
		if (it3->second != nullptr) delete (it3->second);
		++it3;
	}
	mgrBillboards.clear();
}

QeCamera* QeObjectManger::getCamera(int _id, QeAssetXML* _property) {

	std::map<int, QeCamera*>::iterator it = mgrCameras.find(_id);
	if (it != mgrCameras.end())	return it->second;

	QeCamera* newCamera = new QeCamera(key);
	newCamera->init(_property);
	mgrCameras[_id] = newCamera;

	return newCamera;
}

QeLight* QeObjectManger::getLight(int _id, QeAssetXML* _property) {
	std::map<int, QeLight*>::iterator it = mgrLights.find(_id);
	if (it != mgrLights.end())	return it->second;

	QeLight* newLight = new QeLight(key);
	newLight->init(_property);
	mgrLights[_id] = newLight;

	return newLight;
}

QeActivity* QeObjectManger::getActivity(int _id, QeAssetXML* _property) {
	std::map<int, QeActivity*>::iterator it = mgrActivitys.find(_id);
	if (it != mgrActivitys.end())	return it->second;

	QeActivity* newActivity = new QeActivity(key);
	newActivity->init(_property);
	mgrActivitys[_id] = newActivity;

	return newActivity;
}

QeModel* QeObjectManger::getModel(int _id, QeAssetXML* _property) {

	std::map<int, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return it->second;

	if (_property == nullptr) return nullptr;

	QeModel* newModel = new QeModel(key);
	newModel->init(_property);
	mgrModels[newModel->id] = newModel;

	VK->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeBillboard* QeObjectManger::getBillboard(int _id) {

	std::map<int, QeModel*>::iterator it = mgrBillboards.find(_id);
	if (it != mgrBillboards.end())	return (QeBillboard*)it->second;

	QeAssetXML* _property = AST->getXMLNode(2, AST->CONFIG, "billboard");

	QeBillboard* newModel = new QeBillboard(key);
	newModel->init(_property);
	mgrBillboards[_id] = newModel;

	VK->bUpdateDrawCommandBuffers = true;
	return newModel;
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

	std::map<int, QeModel*>::iterator it2 = mgrModels.begin();
	while (it2 != mgrModels.end()) {
		it2->second->update(_time);
		++it2;
	}

	it2 = mgrBillboards.begin();
	while (it2 != mgrBillboards.end()) {
		it2->second->update(_time);
		++it2;
	}
}


void QeObjectManger::cleanupPipeline() {

	std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->cleanupPipeline();
		++it;
	}

	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		it->second->cleanupPipeline();
		++it;
	}
}

void QeObjectManger::recreatePipeline() {

	std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->createPipeline();
		++it;
	}
	
	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		it->second->createPipeline();
		++it;
	}
	VK->bUpdateDrawCommandBuffers = true;
}

std::vector<QeModel*> QeObjectManger::getDrawObject() {

	std::vector<QeModel*> vec;
	std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		vec.push_back(it->second);
		++it;
	}

	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		vec.push_back(it->second);
		++it;
	}
	return vec;
}