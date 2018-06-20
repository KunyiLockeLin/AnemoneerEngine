#include "qeheader.h"

void QePoint::init(QeAssetXML* _property, int _parentOID) {
	initProperty = _property;
	parentOID = _parentOID;
	setProperty();
}

void QePoint::setProperty() {
	oid = 0;
	AST->getXMLiValue(&oid, initProperty, 1, "oid");

	eid = 0;
	AST->getXMLiValue(&eid, initProperty, 1, "eid");
	editProperty = AST->getXMLEditNode(objectType, eid);

	pos = { 0.0f, 0.0f, 0.0f };
	AST->getXMLfValue(&pos.x, initProperty, 1, "posX");
	AST->getXMLfValue(&pos.y, initProperty, 1, "posY");
	AST->getXMLfValue(&pos.z, initProperty, 1, "posZ");

	AST->getXMLbValue(&bFollowPos, initProperty, 1, "followPos");
	AST->getXMLbValue(&bFollowColor, initProperty, 1, "followColor");

	attachOIDs.clear();
	QeAssetXML * node = AST->getXMLNode(initProperty, 1, "attachs");
	if (node != nullptr && node->nexts.size() > 0) {
		for (int index = 0; index < node->nexts.size(); ++index) {
			QePoint * p = OBJMGR->getObject(0, node->nexts[index], oid);
			if (p) attachOIDs.push_back(p->oid);
		}
	}

	attachSkeletonName = "";
	const char* c = AST->getXMLValue(initProperty, 1, "attachskeleton");
	if (c) attachSkeletonName = std::string(c);
}

void QePoint::reset() {
	init(initProperty, parentOID);
}

QeMatrix4x4f QePoint::getAttachMatrix(const char* attachSkeletonName, bool bRotate, bool bScale) {
	return  MATH->translate(pos);
}