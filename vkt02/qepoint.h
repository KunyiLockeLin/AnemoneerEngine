#pragma once

#include "qeheader.h"

class QePoint {

public:
	QePoint(QeObjectMangerKey& _key, QeObjectType _type = eObject_Point) :objectType(_type) {}
	//~QePoint() {}

	int oid;
	int eid;
	QeAssetXML* initProperty;
	QeAssetXML* editProperty;
	QeVector3f pos;
	std::vector<int> attachOIDs;
	int	parentOID;
	std::string attachSkeletonName;
	bool bFollowPos;
	bool bFollowColor;
	const QeObjectType objectType;

	virtual void init(QeAssetXML* _property, int _parentOID);
	virtual void setProperty();
	virtual void reset();
	virtual QeMatrix4x4f getAttachMatrix(const char* attachSkeletonName);
	virtual void updateCompute(float time) {}
	virtual void updateRender(float time) {}
};