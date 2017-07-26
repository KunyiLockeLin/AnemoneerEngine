#include "qelight.h"

void QeLight::init() {
	data.pos = QeVector4f(10, 10, 10, 1);
	data.dir  = QeVector4f(1, 1, 1, 1);
	data.color = QeVector4f(1, 0, 0, 1);
	data.type = eLightPoint;
	data.intensity = 1000;
	data.radius = 50;
}