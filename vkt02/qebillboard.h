#pragma once
#include "qeheader.h"


enum QeBillboardType {
	eBillboardFace,
	eBillboardFaceAndSize,
};


class QeBillboard : public QeModel
{
public:
	// face: (0,0,1)

	QeBillboard(QeObjectMangerKey& _key):QeModel(_key){}
	~QeBillboard() {}

	QeBillboardType type = eBillboardFace;
	int width=0, height=0;
	virtual void setMatModel();
	virtual void update(float time);
};