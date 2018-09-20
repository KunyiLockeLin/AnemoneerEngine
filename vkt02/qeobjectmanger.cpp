#include "qeheader.h"


QeObjectManger::~QeObjectManger() {

	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it = components.begin();
	std::vector<QeComponent*>::iterator it1;

	while (it != components.end()) {
		it1 = it->second.begin();
		while (it1 != it->second.end()) {
			(*it1)->clear();
			delete *it1;
			++it1;
		}
		it->second.clear();
		++it;
	}
	components.clear();

	it = unActiveComponents.begin();
	while (it != unActiveComponents.end()) {
		it1 = it->second.begin();
		while (it1 != it->second.end()) {
			(*it1)->clear();
			delete *it1;
			++it1;
		}
		it->second.clear();
		++it;
	}
	unActiveComponents.clear();

	std::vector<QeObject*>::iterator it2 = objects.begin();
	while (it2 != objects.end()) {
		(*it2)->clear();
		delete *it2;
		++it2;
	}
	objects.clear();

	it2 = unActiveObjects.begin();
	while (it2 != unActiveObjects.end()) {
		(*it2)->clear();
		delete *it2;
		++it1;
	}
	unActiveObjects.clear();
}

void QeObjectManger::clear() {

	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it = components.begin();
	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it11;

	std::vector<QeComponent*>::iterator it1;

	while (it != components.end()) {

		if (it->second.size() ) {
			it11 = unActiveComponents.find(it->first);

			if (it11 == unActiveComponents.end()) {
				unActiveComponents.insert(std::pair<QeComponentType, std::vector<QeComponent*>>(it->first, {}));
				it11 = unActiveComponents.find(it->first);
			}

			it1 = it->second.begin();

			while (it1 != it->second.end()) {
				(*it1)->clear();
				it11->second.push_back(*it1);
				++it1;
			}
			it->second.clear();
		}
		++it;
	}

	std::vector<QeObject*>::iterator it2 = objects.begin();
	while (it2 != objects.end()) {
		(*it2)->clear();
		unActiveObjects.push_back(*it2);
		++it2;
	}
	objects.clear();
}

QeObject* QeObjectManger::spwanObject(QeAssetXML* _property, QeObject* _parent) {

	QeObject* _object = nullptr;

	size_t size = unActiveObjects.size();
	if (size > 0) {
		_object = unActiveObjects[size-1];
		unActiveObjects.pop_back();
	}
	else {
		_object = new QeObject(key);
	}
	objects.push_back(_object);

	_object->initialize(_property, _parent);
	return _object;
}

QeComponent* QeObjectManger::spwanComponent(QeAssetXML* _property, QeObject* _owner) {

	QeComponent* _component = nullptr;

	QeComponentType _type;
	AST->getXMLiValue((int*)&_type, _property, 1, "type");

	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it = unActiveComponents.find(_type);
	if (it!= unActiveComponents.end() && it->second.size()>0) {
		size_t size = it->second.size();
		_component = it->second[size-1];
		it->second.pop_back();
	}
	else {
		switch (_type) {
		case eComponent_transform:
			_component = new QeTransform(key);
			break;
		case eComponent_camera:
			_component = new QeCamera(key);
			break;
		case eComponent_postprocessing:
			//_component = new QePostprocessing();
			break;
		case eComponent_light:
			_component = new QeLight(key);
			break;
		case eComponent_line:
			_component = new QeLine(key);
			break;
		case eComponent_axis:
			_component = new QeAxis(key);
			break;
		case eComponent_grid:
			_component = new QeGrid(key);
			break;
		case eComponent_model:
			_component = new QeModel(key);
			break;
		case eComponent_animation:
			_component = new QeAnimation(key);
			break;
		case eComponent_billboard:
			_component = new QeBillboard(key);
			break;
		case eComponent_cubemap:
			_component = new QeCubemap(key);
			break;
		case eComponent_partical:
			_component = new QeParticle(key);
			break;
		case eComponent_material:
			//_component = new QeMaterial();
			break;
		case eComponent_inputControl:
			_component = new QeInputControl(key);
			break;
		case eComponent_render:
			_component = new QeRender(key);
			break;
		}
	}
	it = components.find(_type);
	if (it != components.end()) {
		it->second.push_back(_component);
	}
	else {
		components.insert(
			std::pair<QeComponentType, std::vector<QeComponent*>>(_type, { _component }));
	}
	_component->initialize(_property, _owner);

	return _component;
}

QeObject* QeObjectManger::findObject(int _oid) {
	
	std::vector<QeObject*>::iterator it = objects.begin();
	while (it != objects.end()) {
		if ((*it)->oid == _oid) {
			return (*it);
		}
		++it;
	}
	return nullptr;
}

QeObject* QeObjectManger::findObject(std::string _name) {
	
	std::vector<QeObject*>::iterator it = objects.begin();
	while (it != objects.end()) {
		if ((*it)->name.compare(_name) == 0) {
			return (*it);
		}
		++it;
	}
	return nullptr;
}

QeComponent* QeObjectManger::findComponent(QeComponentType type, int _oid) {

	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it = components.find(type);

	if (it != components.end()) {
		std::vector<QeComponent*>::iterator it1 = it->second.begin();
		while (it1 != it->second.end()) {
			if ((*it1)->oid == _oid) {
				return (*it1);
			}
			++it1;
		}
		++it;
	}
	return nullptr;
}

bool QeObjectManger::removeObject(QeObject* _object) {
	
	int index = findElementFromVector(objects, _object);

	if (index == INDEX_NONE) return false;

	_object->clear();
	objects.erase(objects.begin()+ index);
	unActiveObjects.push_back(_object);

	return true;
}

bool QeObjectManger::removeComponent(QeComponent* _component) {
	
	std::map< QeComponentType, std::vector<QeComponent*>>::iterator it = components.find(_component->componentType);
	if (it != components.end()) {

		int index = findElementFromVector(it->second, _component);

		if (index == INDEX_NONE) return false;
		_component->clear();
		it->second.erase(it->second.begin()+index);
				
		it = unActiveComponents.find(_component->componentType);
		if (it != unActiveComponents.end()) {
			it->second.push_back(_component);
		}
		else {
			unActiveComponents.insert(std::pair<QeComponentType, std::vector<QeComponent*>>(_component->componentType, { _component }));
		}
		return true;
	}
	return false;
}