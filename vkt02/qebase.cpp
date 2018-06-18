#include "qeheader.h"

void QeBase::init(QeAssetXML* _property) {
	initProperty = _property;
	int i = 0;
	AST->getXMLiValue(i, *initProperty, 1, "id");
	id = i;
	AST->getXMLfValue(pos.x, *initProperty, 1, "posX");
	AST->getXMLfValue(pos.y, *initProperty, 1, "posY");
	AST->getXMLfValue(pos.z, *initProperty, 1, "posZ");

	AST->getXMLiValue(particleID, *initProperty, 1, "paritcleid");
	if (particleID)	particle = OBJMGR->getParticle(particleID, initProperty);
}
void QeBase::updateRender(float time) {}
void QeBase::updateCompute(float time) {}