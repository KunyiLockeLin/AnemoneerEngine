#include "qeheader.h"


QeObjectManger::~QeObjectManger() {
	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		if ((it->second) != nullptr) delete (it->second);
		++it;
	}
	/*
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
	
	it3 = mgrCubes.begin();
	while (it3 != mgrCubes.end()) {
		if (it3->second != nullptr) delete (it3->second);
		++it3;
	}
	mgrCubes.clear();

	it3 = mgrBillboards.begin();
	while (it3 != mgrBillboards.end()) {
		if (it3->second != nullptr) delete (it3->second);
		++it3;
	}
	mgrBillboards.clear();

	it3 = mgrLines.begin();
	while (it3 != mgrLines.end()) {
		if (it3->second != nullptr) delete (it3->second);
		++it3;
	}
	mgrLines.clear();
	*/
}

QeCamera* QeObjectManger::getCamera(int _id, QeAssetXML* _property) {

	//std::map<int, QeCamera*>::iterator it = mgrCameras.find(_id);
	//if (it != mgrCameras.end())	return it->second;
	//if (_id > ID_CAMERA_MAX || _id < ID_CAMERA_MIN) return nullptr;

	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeCamera*)it->second;

	QeCamera* newCamera = new QeCamera(key);
	//mgrCameras[_id] = newCamera;
	mgrObjs[_id] = newCamera;
	newCamera->init(_property);
	newCamera->id = _id;

	return newCamera;
}

QeLight* QeObjectManger::getLight(int _id, QeAssetXML* _property) {

	//if (_id > ID_LIGHT_MAX || _id < ID_LIGHT_MIN) return nullptr;

	//std::map<int, QeLight*>::iterator it = mgrLights.find(_id);
	//if (it != mgrLights.end())	return it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeLight*)it->second;

	QeLight* newLight = new QeLight(key);
	//mgrLights[_id] = newLight;
	mgrObjs[_id] = newLight;
	newLight->init(_property);
	newLight->id = _id;

	return newLight;
}
/*
QeActivity* QeObjectManger::getActivity(int _id, QeAssetXML* _property) {

	//if (_id > ID_ACTIVITY_MAX || _id < ID_ACTIVITY_MIN) return nullptr;

	//std::map<int, QeActivity*>::iterator it = mgrActivitys.find(_id);
	//if (it != mgrActivitys.end())	return it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeActivity*)it->second;

	QeActivity* newActivity = new QeActivity(key);
	//mgrActivitys[_id] = newActivity;
	mgrObjs[_id] = newActivity;
	newActivity->init(_property);

	return newActivity;
}
*/
QeModel* QeObjectManger::getModel(int _id, QeAssetXML* _property) {

	//if (_id > ID_MODEL_MAX || _id < ID_MODEL_MIN) return nullptr;

	//std::map<int, QeModel*>::iterator it = mgrModels.find(_id);
	//if (it != mgrModels.end())	return it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeModel*)it->second;

	if (_property == nullptr) return nullptr;

	QeModel* newModel = new QeModel(key);
	//mgrModels[newModel->id] = newModel;
	mgrObjs[_id] = newModel;
	newModel->init(_property);
	newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeCube* QeObjectManger::getCube(int _id, QeAssetXML* _property) {

	//std::map<int, QeModel*>::iterator it = mgrCubes.find(_id);
	//if (it != mgrCubes.end())	return  (QeCube*)it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeCube*)it->second;

	if (_property == nullptr) return nullptr;

	QeCube* newModel = new QeCube(key);
	//mgrCubes[newModel->id] = newModel;
	mgrObjs[_id] = newModel;
	newModel->init(_property);
	newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeBillboard* QeObjectManger::getBillboard(int _id,  QeAssetXML* _property) {

	//std::map<int, QeModel*>::iterator it = mgrBillboards.find(_id);
	//if (it != mgrBillboards.end())	return (QeBillboard*)it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeBillboard*)it->second;

	QeBillboard* newModel = new QeBillboard(key);
	//mgrBillboards[_id] = newModel;
	mgrObjs[_id] = newModel;
	newModel->init(_property);
	newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeLine* QeObjectManger::getLine(int _id, QeAssetXML* _property, const char* lineType) {

	//std::map<int, QeModel*>::iterator it = mgrLines.find(_id);
	//if (it != mgrLines.end())	return (QeLine*)it->second;
	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeLine*)it->second;

	QeLine* newModel = new QeLine(key);
	if (lineType) newModel->lineType = lineType;
	//mgrLines[_id] = newModel;
	mgrObjs[_id] = newModel;
	newModel->init(_property);
	newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeParticle* QeObjectManger::getParticle(int _id, QeAssetXML* _property) {

	std::map<int, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeParticle*)it->second;

	QeParticle* newModel = new QeParticle(key);
	mgrObjs[_id] = newModel;
	newModel->init(_property);
	newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

void QeObjectManger::updateRender(float _time) {
	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateRender(_time);
		++it;
	}

	/*
	std::map<int, QeCamera*>::iterator it = mgrCameras.begin();
	while (it != mgrCameras.end()) {
		it->second->updateRender(_time);
		++it;
	}

	std::map<int, QeLight*>::iterator it1 = mgrLights.begin();
	while (it1 != mgrLights.end()) {
		it1->second->updateRender(_time);
		++it1;
	}

	std::map<int, QeModel*>::iterator it2 = mgrModels.begin();
	while (it2 != mgrModels.end()) {
		it2->second->updateRender(_time);
		++it2;
	}

	it2 = mgrCubes.begin();
	while (it2 != mgrCubes.end()) {
		it2->second->updateRender(_time);
		++it2;
	}

	it2 = mgrBillboards.begin();
	while (it2 != mgrBillboards.end()) {
		it2->second->updateRender(_time);
		++it2;
	}

	it2 = mgrLines.begin();
	while (it2 != mgrLines.end()) {
		it2->second->updateRender(_time);
		++it2;
	}
	*/
}

void QeObjectManger::updateCompute(float _time) {

	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateCompute(_time);
		++it;
	}

	/*std::map<int, QeCamera*>::iterator it = mgrCameras.begin();
	while (it != mgrCameras.end()) {
		it->second->updateCompute(_time);
		++it;
	}

	std::map<int, QeLight*>::iterator it1 = mgrLights.begin();
	while (it1 != mgrLights.end()) {
		it1->second->updateCompute(_time);
		++it1;
	}

	std::map<int, QeModel*>::iterator it2 = mgrModels.begin();
	while (it2 != mgrModels.end()) {
		it2->second->updateCompute(_time);
		++it2;
	}
	
	it2 = mgrCubes.begin();
	while (it2 != mgrCubes.end()) {
		it2->second->updateCompute(_time);
		++it2;
	}

	it2 = mgrBillboards.begin();
	while (it2 != mgrBillboards.end()) {
		it2->second->updateCompute(_time);
		++it2;
	}

	it2 = mgrLines.begin();
	while (it2 != mgrLines.end()) {
		it2->second->updateCompute(_time);
		++it2;
	}*/
}

void QeObjectManger::cleanupPipeline() {

	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->cleanupPipeline();
		++it;
	}
	/*std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->cleanupPipeline();
		++it;
	}
	
	it = mgrCubes.begin();
	while (it != mgrCubes.end()) {
		it->second->cleanupPipeline();
		++it;
	}

	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		it->second->cleanupPipeline();
		++it;
	}

	it = mgrLines.begin();
	while (it != mgrLines.end()) {
		it->second->cleanupPipeline();
		++it;
	}*/
}

void QeObjectManger::recreatePipeline() {

	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->createPipeline();
		++it;
	}

	/*std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->createGraphicsPipeline();
		++it;
	}
	it = mgrCubes.begin();
	while (it != mgrCubes.end()) {
		it->second->createGraphicsPipeline();
		++it;
	}
	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		it->second->createGraphicsPipeline();
		++it;
	}
	it = mgrLines.begin();
	while (it != mgrLines.end()) {
		it->second->createGraphicsPipeline();
		++it;
	}*/
	VP->bUpdateDrawCommandBuffers = true;
}


void QeObjectManger::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {
	
	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}
	/*std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}
	it = mgrCubes.begin();
	while (it != mgrCubes.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}
	it = mgrBillboards.begin();
	while (it != mgrBillboards.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}
	it = mgrLines.begin();
	while (it != mgrLines.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}*/
}

void QeObjectManger::updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	std::map<int, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateComputeCommandBuffer(drawCommandBuffer);
		++it;
	}
}