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
	vec = MATH->normalize( pos - center );
	MATH->getAnglefromVector(vec, polarAngle, azimuthalAngle);

	up = { 0.0f, 0.0f, 1.0f };
	AST->getXMLfValue(&up.x, initProperty, 1, "upX");
	AST->getXMLfValue(&up.y, initProperty, 1, "upY");
	AST->getXMLfValue(&up.z, initProperty, 1, "upZ");

	bUpdate = true;
}


void QeLight::updateCompute(float time) {

	if (speed) {
		float angle = time * speed;

		//QeVector3f vec1 = MATH->normalize( MATH->cross(vec, up) );
		//vec1 = {0,0,1};
		polarAngle += vec.z*angle;
		azimuthalAngle += MATH->length(QeVector2f(vec.x, vec.y))*angle;

		while (polarAngle > 360) polarAngle -= 360;
		while (polarAngle < -360) polarAngle += 360;
		while (azimuthalAngle > 360) azimuthalAngle -= 360;
		while (azimuthalAngle < -360) azimuthalAngle += 360;

		MATH->rotatefromCenter(center, pos, polarAngle, azimuthalAngle);

		bufferData.pos = pos;
		bUpdate = true;
	}
}

void QeLight::updateRender(float time) {}