#include "qeheader.h"


QeObjectManger::~QeObjectManger() {
	std::map<int, QePoint*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		if ((it->second) != nullptr) delete (it->second);
		++it;
	}
	mgrObjs.clear();
	std::map<int, QeModel*>::iterator it1 = mgrModels.begin();
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

QePoint* QeObjectManger::getObject(int _oid, QeAssetXML* _property, int _parentOID) {

	if (!_oid && !_property)	return nullptr;
	else if (!_oid)				AST->getXMLiValue(&_oid, _property, 1, "oid");

	std::map<int, QePoint*>::iterator it = mgrObjs.find(_oid);
	if (it != mgrObjs.end())	return it->second;

	std::map<int, QeModel*>::iterator it1 = mgrModels.find(_oid);
	if (it1 != mgrModels.end())	return it1->second;

	std::vector<QeModel*>::iterator it2 = mgrAlphaModels.begin();
	if (it2 != mgrAlphaModels.end()) {
		if ((*it2)->oid == _oid)	return *it2;
		++it2;
	}

	if (!_property) return nullptr;

	bool bAlpha = false;
	bool bCheckAlpha = false;
	bool bRedner = false;
	int type = _oid / 1000;

	switch (type) {
	case eObject_Point:
	case eObject_Camera:
	case eObject_Light:
		break;

	case eObject_Line:
	case eObject_Cubemap:
		bRedner = true;
		break;

	case eObject_Billboard:
		bRedner = true;
		bCheckAlpha = true;
		break;
	case eObject_Particle:
		bRedner = true;
		break;
	case eObject_Model:
		bRedner = true;
		bCheckAlpha = true;
		break;
	case eObject_Render:
		bRedner = true;
		break;

	default:
		return nullptr;
	}

	int _eid = 0;
	if (bRedner && bCheckAlpha && AST->getXMLiValue(&_eid, _property, 1, "eid")) {

		QeAssetXML* node = AST->getXMLEditNode(QeObjectType(type), _eid);

		if (node)	AST->getXMLbValue(&bAlpha, node, 1, "alpha");
		else		return nullptr;
	}

	QePoint* newObject = nullptr;

	switch (type) {
	case eObject_Point:
		newObject = new QePoint(key);
		break;
	case eObject_Camera:
		newObject = new QeCamera(key);
		break;
	case eObject_Light:
		newObject = new QeLight(key);
		break;
	case eObject_Line:
		newObject = new QeLine(key);
		break;
	case eObject_Billboard:
		newObject = new QeBillboard(key);
		break;
	case eObject_Cubemap:
		newObject = new QeCubemap(key);
		break;
	case eObject_Particle:
		newObject = new QeParticle(key);
		break;
	case eObject_Model:
		newObject = new QeModel(key);
		break;
	case eObject_Render:
		newObject = new QeRender(key);
		break;
	}

	if (!newObject) return newObject;

	if (!bRedner) mgrObjs[_oid] = newObject;
	else if (bAlpha) mgrAlphaModels.push_back((QeModel*)newObject);
	else			 mgrModels[_oid] = (QeModel*)newObject;

	newObject->init(_property, _parentOID);
	return newObject;
}

void QeObjectManger::sortAlphaModels(QeCamera * camera) {

	if (!camera) return;
	
	size_t size = mgrAlphaModels.size();
	
	for (size_t i = 0; i<size ;++i) {
		for (size_t j = i + 1; j < size; ++j) {
			float dis1 = MATH->length( camera->pos - mgrAlphaModels[i]->pos);
			float dis2 = MATH->length(camera->pos - mgrAlphaModels[j]->pos);

			if (dis1 < dis2) {
				std::swap(mgrAlphaModels[i], mgrAlphaModels[j]);
			}
		}
	}
}

void QeObjectManger::updateRender(float _time) {
	std::map<int, QePoint*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateRender(_time);
		++it;
	}
	std::map<int, QeModel*>::iterator it1 = mgrModels.begin();
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

	std::map<int, QePoint*>::iterator it = mgrObjs.begin();
	while (it != mgrObjs.end()) {
		it->second->updateCompute(_time);
		++it;
	}
	std::map<int, QeModel*>::iterator it1 = mgrModels.begin();
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

	//VK->cleanupPipeline();
}

void QeObjectManger::recreatePipeline() {

	std::map<int, QeModel*>::iterator it = mgrModels.begin();
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


void QeObjectManger::updateDrawCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet, VkRenderPass& renderPass) {
	
	std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateDrawCommandBuffer(commandBuffer, camera, commonDescriptorSet, renderPass);
		++it;
	}
	sortAlphaModels(camera);
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->updateDrawCommandBuffer(commandBuffer, camera, commonDescriptorSet, renderPass);
		++it1;
	}
}

void QeObjectManger::updateComputeCommandBuffer(VkCommandBuffer& commandBuffer, QeCamera* camera, QeDataDescriptorSet* commonDescriptorSet) {

	std::map<int, QeModel*>::iterator it = mgrModels.begin();
	while (it != mgrModels.end()) {
		it->second->updateComputeCommandBuffer(commandBuffer, camera, commonDescriptorSet);
		++it;
	}
	std::vector<QeModel*>::iterator it1 = mgrAlphaModels.begin();
	while (it1 != mgrAlphaModels.end()) {
		(*it1)->updateComputeCommandBuffer(commandBuffer, camera, commonDescriptorSet);
		++it1;
	}
}