#include "qeheader.h"

void QeLight::init() {
	data.pos = QeVector4f(5, 0, 0, 1);
	data.dir  = QeVector4f(1, 1, 1, 1);
	data.color = QeVector4f(1, 0, 0, 1);
	data.type = eLightPoint;
	data.intensity = 1000;
	data.coneAngle = 90;

	billboard = OBJMGR->getBillboard(0);
	billboard->modelData->pMaterial->value.diffuse = data.color;
}


void QeLight::update(float time) {

	float angle = -time * 30;
	QeMatrix4x4f mat;
	mat *= MATH->rotateZ(angle);
	data.pos = mat*data.pos;
	billboard->pos = data.pos;
}