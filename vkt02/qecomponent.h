#pragma once

#include "qeheader.h"

enum QeComponentType {
	eComponent_transform=1,
	eComponent_camera=2,
	eComponent_postprocessing=3,
	eComponent_light=4,
	eComponent_line=5,
	eComponent_model=6,
	eComponent_animation=7,
	eComponent_billboard=8,
	eComponent_cubemap=9,
	eComponent_partical=10,
	eComponent_material=11,
	eComponent_inputControl=12,
	eComponent_axis=13,
	eComponent_grid=14,
	eComponent_render=15,
};

class QeComponent
{
public:
	QeComponent(QeObjectMangerKey& _key){}
	~QeComponent() {}

	QeComponentType componentType;
	
	int oid, eid;
	QeAssetXML* initProperty = nullptr;
	QeObject* owner = nullptr;

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void clear() {}

	virtual void update1() {}
	virtual void update2() {}
};

