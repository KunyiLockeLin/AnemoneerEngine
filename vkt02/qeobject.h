#pragma once

#include "qeheader.h"


class QeObject
{
public:
	QeObject() {}
	~QeObject() {}

	int oid, eid, parentOID;
	std::string name;

	std::vector<QeComponent*> components;

	void init(QeAssetXML* _property, int _parentOID);
	void update1();
	void update2();
};

