#include "qeheader.h"


void QeBillboard::setMatModel() {
	
	//type = eBillboardFaceAndSize;

	QeMatrix4x4f mat;
	mat *= MATH->translate(pos);
	//size = { 3,3,3 };
	mat *= MATH->scale(size);

	ubo.model = mat;
	ubo.param.y = float(type);
}

void QeBillboard::update(float time) { updateUniformBuffer(); }