#include "qeheader.h"


QeObjectManger::~QeObjectManger() {
	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		if ((it->second) != nullptr) delete (it->second);
		++it;
	}
	mgrObjs.clear();
	std::map<uint16_t, QeModel*>::iterator it1 = mgrModels.begin();
	while (it1 != mgrModels.end()) {
		if ((it1->second) != nullptr) delete (it1->second);
		++it1;
	}
	mgrModels.clear();
	std::vector<QeModel*>::iterator it2 = mgrAlphaModels.begin();
	while (it2 != mgrAlphaModels.end()) {
		if (*it2 != nullptr) delete *it2;
		++it2;
	}
	mgrAlphaModels.clear();
}

QeBase* QeObjectManger::getPoint(int _id, QeAssetXML* _property) {

	//std::map<int, QeCamera*>::iterator it = mgrCameras.find(_id);
	//if (it != mgrCameras.end())	return it->second;
	//if (_id > ID_CAMERA_MAX || _id < ID_CAMERA_MIN) return nullptr;

	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return it->second;

	if (!_property) return nullptr;

	QeBase* newPoint = new QeBase(key);
	//mgrCameras[_id] = newCamera;
	mgrObjs[_id] = newPoint;
	newPoint->init(_property);
	//newCamera->id = _id;

	return newPoint;
}

QeCamera* QeObjectManger::getCamera(int _id, QeAssetXML* _property) {

	//std::map<int, QeCamera*>::iterator it = mgrCameras.find(_id);
	//if (it != mgrCameras.end())	return it->second;
	//if (_id > ID_CAMERA_MAX || _id < ID_CAMERA_MIN) return nullptr;

	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeCamera*)it->second;

	if (!_property) return nullptr;

	QeCamera* newCamera = new QeCamera(key);
	//mgrCameras[_id] = newCamera;
	mgrObjs[_id] = newCamera;
	newCamera->init(_property);
	//newCamera->id = _id;

	return newCamera;
}

QeLight* QeObjectManger::getLight(int _id, QeAssetXML* _property) {

	//if (_id > ID_LIGHT_MAX || _id < ID_LIGHT_MIN) return nullptr;

	//std::map<int, QeLight*>::iterator it = mgrLights.find(_id);
	//if (it != mgrLights.end())	return it->second;
	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.find(_id);
	if (it != mgrObjs.end())	return (QeLight*)it->second;

	if (!_property) return nullptr;

	QeLight* newLight = new QeLight(key);
	//mgrLights[_id] = newLight;
	mgrObjs[_id] = newLight;
	newLight->init(_property);
	//newLight->id = _id;

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

	std::map<uint16_t, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return (QeModel*)it->second;

	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	if (it1 != mgrAlphaModels.end()) {
		if ((*it1)->id == _id)	return *it1;
		++it1;
	}

	if (_property == nullptr) return nullptr;

	QeModel* newModel = new QeModel(key);
	//mgrModels[newModel->id] = newModel;
	
	bool bAlpha = false;
	const char *c = AST->getXMLValue(_property, 1, "alpha");
	if (c != nullptr)	bAlpha = atoi(c);

	if (bAlpha)	addAlphaModels(newModel);
	else		mgrModels[_id] = newModel;

	newModel->init(_property);
	//newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeCube* QeObjectManger::getCube(int _id, QeAssetXML* _property) {

	//std::map<int, QeModel*>::iterator it = mgrCubes.find(_id);
	//if (it != mgrCubes.end())	return  (QeCube*)it->second;
	std::map<uint16_t, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return (QeCube*)it->second;

	if (_property == nullptr) return nullptr;

	QeCube* newModel = new QeCube(key);
	//mgrCubes[newModel->id] = newModel;
	mgrModels[_id] = newModel;
	newModel->init(_property);
	//newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeBillboard* QeObjectManger::getBillboard(int _id,  QeAssetXML* _property) {

	//std::map<int, QeModel*>::iterator it = mgrBillboards.find(_id);
	//if (it != mgrBillboards.end())	return (QeBillboard*)it->second;
	std::map<uint16_t, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return (QeBillboard*)it->second;

	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	if (it1 != mgrAlphaModels.end()) {
		if( (*it1)->id == _id )	return (QeBillboard*)*it1;
		++it1;
	}

	if (!_property) return nullptr;

	QeBillboard* newModel = new QeBillboard(key);
	//mgrBillboards[_id] = newModel;
	
	bool bAlpha = false;
	const char *c = AST->getXMLValue(_property, 1, "alpha");
	if (c != nullptr )	bAlpha = atoi(c);
	
	if(bAlpha)	addAlphaModels(newModel);
	else		mgrModels[_id] = newModel;

	newModel->init(_property);
	//newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeLine* QeObjectManger::getLine(int _id, QeAssetXML* _property, const char* lineType) {

	//std::map<int, QeModel*>::iterator it = mgrLines.find(_id);
	//if (it != mgrLines.end())	return (QeLine*)it->second;
	std::map<uint16_t, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return (QeLine*)it->second;

	QeLine* newModel = new QeLine(key);
	if (lineType) newModel->lineType = lineType;
	//mgrLines[_id] = newModel;
	mgrModels[_id] = newModel;
	newModel->init(_property);
	//newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

QeParticle* QeObjectManger::getParticle(int _id, QeAssetXML* _property) {

	std::map<uint16_t, QeModel*>::iterator it = mgrModels.find(_id);
	if (it != mgrModels.end())	return (QeParticle*)it->second;

	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	if (it1 != mgrAlphaModels.end()) {
		if ((*it1)->id == _id)	return (QeParticle*)*it1;
		++it1;
	}

	if (!_property) return nullptr;

	QeParticle* newModel = new QeParticle(key);
	//mgrBillboards[_id] = newModel;
	const char * c = AST->getXMLValue(_property, 1, "paritcleEid");

	bool bAlpha = false;
	if (c) {
		QeAssetParticleRule* p = AST->getParticle(c);
		if (p->alpha_born_size_x.x)	bAlpha = true;
	}

	if (bAlpha)	addAlphaModels(newModel);
	else		mgrModels[_id] = newModel;
	newModel->id = _id;
	newModel->init(_property);
	//newModel->id = _id;

	VP->bUpdateDrawCommandBuffers = true;
	return newModel;
}

void QeObjectManger::sortAlphaModels() {

	QeCamera * camera = VP->viewports[VP->currentCommandViewport]->camera;
	if (!camera) return;
	
	size_t size = mgrAlphaModels.size();
	
	for (size_t i = 0; i<size ;++i) {
		for (size_t j = i + 1; j < size; ++j) {
			float dis1 = MATH->distance( camera->pos, mgrAlphaModels[i]->pos);
			float dis2 = MATH->distance(camera->pos, mgrAlphaModels[j]->pos);

			if (dis1 < dis2) {
				std::swap(mgrAlphaModels[i], mgrAlphaModels[j]);
			}
		}
	}
}

void QeObjectManger::addAlphaModels(QeModel* model) {
	
	mgrAlphaModels.push_back(model);
	VP->bUpdateDrawCommandBuffers = true;
}

void QeObjectManger::updateRender(float _time) {
	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateRender(_time);
		++it;
	}
	std::map<uint16_t, QeModel*>::iterator it1 = mgrModels.begin();
	while (it1 != mgrModels.end()) {
		it1->second->updateRender(_time);
		++it1;
	}
	std::vector<QeModel*>::iterator it2 = mgrAlphaModels.begin();
	while (it2 != mgrAlphaModels.end()) {
		(*it2)->updateRender(_time);
		++it2;
	}
}

void QeObjectManger::updateCompute(float _time) {

	std::map<uint16_t, QeBase*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateCompute(_time);
		++it;
	}
	std::map<uint16_t, QeModel*>::iterator it1 = mgrModels.begin();
	while (it1 != mgrModels.end()) {
		it1->second->updateCompute(_time);
		++it1;
	}
	std::vector<QeModel*>::iterator it2 = mgrAlphaModels.begin();
	while (it2 != mgrAlphaModels.end()) {
		(*it2)->updateCompute(_time);
		++it2;
	}
}

void QeObjectManger::cleanupPipeline() {

	std::map<uint16_t, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->cleanupPipeline();
		++it;
	}
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->cleanupPipeline();
		++it1;
	}
}

void QeObjectManger::recreatePipeline() {

	std::map<uint16_t, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateShaderData();
		it->second->createPipeline();
		++it;
	}
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->updateShaderData();
		(*it1)->createPipeline();
		++it1;
	}
	VP->bUpdateDrawCommandBuffers = true;
}


void QeObjectManger::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {
	
	std::map<uint16_t, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateDrawCommandBuffer(drawCommandBuffer);
		++it;
	}
	sortAlphaModels();
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->updateDrawCommandBuffer(drawCommandBuffer);
		++it1;
	}
}

void QeObjectManger::updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	std::map<uint16_t, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateComputeCommandBuffer(drawCommandBuffer);
		++it;
	}
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->updateComputeCommandBuffer(drawCommandBuffer);
		++it1;
	}
}