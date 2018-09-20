#include "qeheader.h"


void QeObject::initialize(QeAssetXML* _property, QeObject* _parent) {
	
	initProperty = _property;
	parent = _parent;
	AST->getXMLiValue(&oid, initProperty, 1, "oid");
	AST->getXMLiValue(&eid, initProperty, 1, "eid");
	name = AST->getXMLValue(initProperty, 1, "name");

	QeAssetXML * node = AST->getXMLNode(initProperty, 1, "components");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			components.push_back(OBJMGR->spwanComponent(node->nexts[index], this));
		}
	}

	node = AST->getXMLNode(initProperty, 1, "children");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			children.push_back(OBJMGR->spwanObject(node->nexts[index], this));
		}
	}
}

void QeObject::clear() {
	std::vector<QeComponent*>::iterator it = components.begin();
	while (it != components.end()) {
		OBJMGR->removeComponent(*it);
		++it;
	}
	components.clear();

	std::vector<QeObject*>::iterator it1 = children.begin();
	while (it1 != children.end()) {
		OBJMGR->removeObject(*it1);
		++it1;
	}
	children.clear();

	if (!parent) {
		eraseElementFromVector(SCENE->objects, this);
		parent = nullptr;
	}
}

void QeObject::update1() {
	std::vector<QeComponent*>::iterator it = components.begin();
	while (it != components.end()) {
		(*it)->update1();
		++it;
	}
	std::vector<QeObject*>::iterator it1 = children.begin();
	while (it1 != children.end()) {
		(*it1)->update1();
		++it1;
	}
}

void QeObject::update2() {
	std::vector<QeComponent*>::iterator it = components.begin();
	while (it != components.end()) {
		(*it)->update2();
		++it;
	}
	std::vector<QeObject*>::iterator it1 = children.begin();
	while (it1 != children.end()) {
		(*it1)->update2();
		++it1;
	}
}