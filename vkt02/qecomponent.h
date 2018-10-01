#pragma once

#include "qeheader.h"

class QeComponent
{
public:
	QeComponent(QeObjectMangerKey& _key){}
	~QeComponent() {}

	QeComponentType componentType;
	
	int oid, eid;
	QeAssetXML* initProperty = nullptr;
	QeObject* owner = nullptr;
	std::string name;

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void clear() {}

	virtual void update1() {}
	virtual void update2() {}
};

