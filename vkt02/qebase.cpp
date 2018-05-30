#include "qeheader.h"

void QeBase::init(QeAssetXML* _property) {
	initProperty = _property;
	int i = 0;
	AST->setXMLValue(i, *_property, 1, "id");
	id = i;
	AST->setXMLValue(pos.x, *_property, 1, "posX");
	AST->setXMLValue(pos.y, *_property, 1, "posY");
	AST->setXMLValue(pos.z, *_property, 1, "posZ");

	const char * c = AST->setXMLValue(*_property, 1, "paritcleid");
	if (c)	particle = OBJMGR->getParticle(atoi(c), nullptr);
}
void QeBase::updateRender(float time) {
	if (particle) particle->pos = pos;
}
void QeBase::updateCompute(float time) {

}