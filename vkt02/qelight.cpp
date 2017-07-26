#include "qelight.h"

void QeLight::init() {
	data.pos = QeVector3f(5, 5, 5);
	data.dir  = QeVector3f(1, 1, 1);
	data.color = QeVector3f(1, 1, 1);
	data.type = eLightPoint;
	data.intensity = 100;
	data.radius = 30;
}