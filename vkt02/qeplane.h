#pragma once
#include "qeheader.h"


enum QePlaneType {
	ePlane_3D=0,
	ePlane_billboard = 1,
	ePlane_2D = 2,
};

class QePlane : public QeModel
{
public:
	// face: (0,0,1)

	QePlane(QeObjectMangerKey& _key) :QeModel(_key) {}
	//~QePlane() {}
	QePlaneType planeType;
	int targetCameraOID;
	bool bUpdateTargetCameraOID;

	virtual void initialize(QeAssetXML* _property, QeObject* _owner);
	virtual void update1();
	virtual QeDataDescriptorSetModel createDescriptorSetModel();
};