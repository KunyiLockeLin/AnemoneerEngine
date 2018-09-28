#include "qeheader.h"


void QeScene::initialize(QeAssetXML* _property) {
	initProperty = _property;

	AST->getXMLiValue(&eid, initProperty, 1, "eid");
	name = initProperty->key;

	for (int index = 0; index < initProperty->nexts.size(); ++index) {
		objects.push_back(OBJMGR->spwanObject(initProperty->nexts[index], nullptr));
	}
}

void QeScene::clear() {
	objects.clear();
}

void QeScene::update1() {
	std::vector<QeObject*>::iterator it = objects.begin();
	while (it != objects.end()) {
		(*it)->update1();
		++it;
	}
}

void QeScene::update2() {
	std::vector<QeObject*>::iterator it = objects.begin();
	while (it != objects.end()) {
		(*it)->update2();
		++it;
	}
}

void QeScene::loadScene(int _eid) {
	OBJMGR->clear();
	clear();
	GRAP->initialize();
	initialize(AST->getXMLEditNode(eScene, _eid));
}
