#include "qeheader.h"



void QeObject::init(QeAssetXML* _property, int _parentOID) {
	std::vector<QeComponent*>::iterator it = components.begin();
	while ( it != components.end() ) {
		(*it)->init(_property, oid);
		++it;
	}
}

void QeObject::update1() {
	std::vector<QeComponent*>::iterator it = components.begin();
	while (it != components.end()) {
		(*it)->update1();
		++it;
	}
}

void QeObject::update2() {
	std::vector<QeComponent*>::iterator it = components.begin();
	while (it != components.end()) {
		(*it)->update2();
		++it;
	}
}