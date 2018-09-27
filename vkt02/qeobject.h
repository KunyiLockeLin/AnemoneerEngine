#pragma once

#include "qeheader.h"


class QeObject
{
public:
	QeObject(QeObjectMangerKey& _key) {}
	~QeObject() {}

	int oid, eid;
	QeObject* parent = nullptr;
	QeAssetXML* initProperty;
	std::string name;

	QeTransform* transform;

	std::vector<QeComponent*> components;
	std::vector<QeObject*> children;

	virtual void initialize(QeAssetXML* _property, QeObject* _parent);
	virtual void clear();

	virtual void update1();
	virtual void update2();
};