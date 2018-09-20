#pragma once

#include "qeheader.h"


class QeObjectMangerKey {
	friend class QeObjectManger;

private:
	QeObjectMangerKey() {}
};

class QeObjectManger {

private:
	QeObjectMangerKey key;

	std::vector<QeObject*> objects;
	std::vector<QeObject*> unActiveObjects;

	std::map< QeComponentType, std::vector<QeComponent*>> components;
	std::map< QeComponentType, std::vector<QeComponent*>> unActiveComponents;

public:

	QeObjectManger(QeGlobalKey& _key) {}
	~QeObjectManger();	

	QeObject* spwanObject( QeAssetXML* _property, QeObject* _parent = nullptr);
	QeComponent* spwanComponent(QeAssetXML* _property, QeObject* _owner = nullptr);
	QeObject* findObject(int _oid);
	QeObject* findObject(std::string name);
	QeComponent* findComponent(QeComponentType type, int _oid);
	bool removeObject(QeObject* _object);
	bool removeComponent(QeComponent* component);

	void clear();
};