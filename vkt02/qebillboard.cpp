#include "qeheader.h"


void QeBillboard::setMatModel() {
	QeMatrix4x4f mat;

	size.x = 10;
	size.y = 10;
	size.z = 10;
	mat *= MATH->translate(pos);
	mat *= MATH->scale(size);

	ubo.model = mat;
	ubo.param1 = float(type);
}

void QeBillboard::update(float time) { updateUniformBuffer(); }