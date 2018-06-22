#include "qeheader.h"


void QeLight::setProperty() {
	QePoint::setProperty();

	bufferData.pos = pos;
	bufferData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	AST->getXMLfValue(&bufferData.color.x, initProperty, 1, "r");
	AST->getXMLfValue(&bufferData.color.y, initProperty, 1, "g");
	AST->getXMLfValue(&bufferData.color.z, initProperty, 1, "b");

	bufferData.dir = { 1.0f, 1.0f, 1.0f, 1.0f };
	AST->getXMLfValue(&bufferData.dir.x, initProperty, 1, "dirX");
	AST->getXMLfValue(&bufferData.dir.y, initProperty, 1, "dirY");
	AST->getXMLfValue(&bufferData.dir.z, initProperty, 1, "dirZ");

	bufferData.param = { 0.0f, 1000.0f, 90.0f, 1.0f };
	AST->getXMLfValue(&bufferData.param.x, initProperty, 1, "type");
	AST->getXMLfValue(&bufferData.param.y, initProperty, 1, "intensity");
	AST->getXMLfValue(&bufferData.param.z, initProperty, 1, "coneAngle");

	speed = 0;
	AST->getXMLfValue(&speed, initProperty, 1, "speed");

	center = { 0.0f, 0.0f, 0.0f };
	AST->getXMLfValue(&center.x, initProperty, 1, "centerX");
	AST->getXMLfValue(&center.y, initProperty, 1, "centerY");
	AST->getXMLfValue(&center.z, initProperty, 1, "centerZ");

	up = { 0.0f, 0.0f, 1.0f };
	AST->getXMLfValue(&up.x, initProperty, 1, "axisX");
	AST->getXMLfValue(&up.y, initProperty, 1, "axisY");
	AST->getXMLfValue(&up.z, initProperty, 1, "axisZ");
	up = MATH->normalize(MATH->cross(up, (pos - center)) );
	bUpdate = true;
}


void QeLight::updateCompute(float time) {

	if (speed) {

		QeMatrix4x4f mat;
		mat *= MATH->translate(center);
		mat *= MATH->rotate(time * speed, up);
		QeVector4f vec4 = QeVector4f({ pos - center }, 1.0f);
		pos = mat * vec4;

		bufferData.pos = pos;
		bUpdate = true;
	}
}

void QeLight::updateRender(float time) {}