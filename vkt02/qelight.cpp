#include "qelight.h"

void QeLight::init() {
	data.pos = QeVector3f(1, 1, 1);
	data.dir  = QeVector3f(1, 0, 0);
	data.color = QeVector3f(1, 0, 0);
	data.type = eLightPoint;
	data.intensity = 1000;
	data.radius = 50;
}