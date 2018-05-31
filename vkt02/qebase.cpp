#include "qeheader.h"

void QeBase::init(QeAssetXML* _property) {
	initProperty = _property;
	int i = 0;
	AST->getXMLiValue(i, *_property, 1, "id");
	id = i;
	AST->getXMLfValue(pos.x, *_property, 1, "posX");
	AST->getXMLfValue(pos.y, *_property, 1, "posY");
	AST->getXMLfValue(pos.z, *_property, 1, "posZ");

	const char * c = AST->getXMLValue(_property, 1, "paritcleid");
	if (c)	particle = OBJMGR->getParticle(atoi(c), _property);
}
void QeBase::updateRender(float time) {
	//if (particle) particle->pos = pos;
}
void QeBase::updateCompute(float time) {

}