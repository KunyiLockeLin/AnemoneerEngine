#pragma once

#include "qeheader.h"

enum QeComponentType {
	eComponent_transform,
	eComponent_camera,
	eComponent_postporcessing,
	eComponent_light,
	eComponent_line,
	eComponent_axis,
	eComponent_grid,
	eComponent_model,
	eComponent_animation,
	eComponent_billboard,
	eComponent_cubemap,
	eComponent_partical,
	eComponent_material,
	eComponent_code,
};

class QeComponent
{
public:
	QeComponent();
	~QeComponent();

	QeComponentType componentType;
	
	int oid, parentOID;

	void init(QeAssetXML* _property, int _parentOID) {}
	void update1() {}
	void update2() {}
};

