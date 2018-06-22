#include "qeheader.h"


void QeCamera::setProperty() {
	QePoint::setProperty();
	//type = eCameraThirdPerson;

	center = { 0.0f, 0.0f, 0.0f };
	AST->getXMLfValue(&center.x, initProperty, 1, "centerX");
	AST->getXMLfValue(&center.y, initProperty, 1, "centerY");
	AST->getXMLfValue(&center.z, initProperty, 1, "centerZ");
	face = MATH->normalize(center - pos);
	//QeVector3f vec = { pos - center };
	//MATH->getAnglefromVector(vec, polarAngle, azimuthalAngle);

	up = { 0.0f, 0.0f, 1.0f };
	AST->getXMLfValue(&up.x, initProperty, 1, "upX");
	AST->getXMLfValue(&up.y, initProperty, 1, "upY");
	AST->getXMLfValue(&up.z, initProperty, 1, "upZ");

	speed = 0.5f;
	AST->getXMLfValue(&speed, initProperty, 1, "speed");

	cullingDistance = 0;
	AST->getXMLiValue(&cullingDistance, initProperty, 1, "culling");

	fov = 45.0f;
	AST->getXMLfValue(&fov, initProperty, 1, "fov");

	fnear = 0.1f;
	AST->getXMLfValue(&fnear, initProperty, 1, "near");

	ffar = 1000.0f;
	AST->getXMLfValue(&ffar, initProperty, 1, "far");

	bUpdate = true;
}

/*void QeCamera::rotatePos(float _angle, QeVector3f _axis) {

	//if (type == eCameraThirdPerson) return;

	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = target - pos;
	QeMatrix4x4f mat = MATH->rotate(_angle*speed, _axis);
	mat *= MATH->translate(pos);
	QeVector4f v4(vec, 1);
	target = mat*v4;
}*/

void QeCamera::rotateTarget(float _angle, QeVector2f _axis) {

	QeVector3f vec = { pos - center };
	float polarAngle, azimuthalAngle;
	MATH->getAnglefromVector(vec, polarAngle, azimuthalAngle);

	if ( (polarAngle < 1 && _angle < 0)|| (polarAngle > 179 && _angle > 0) ) return;
	
	_angle *= speed;
	QeMatrix4x4f mat;
	mat *= MATH->translate(center);

	if ( _axis.x) {
		float f = polarAngle + _angle;
		if (f < 1)			_angle = -polarAngle;
		else if (f > 179)	_angle = 180 - polarAngle;

		QeVector3f up2 = MATH->normalize(MATH->cross(up, vec));
		mat *= MATH->rotate(_angle, up2);
	}
	else	mat *= MATH->rotate(_angle, up);
	
	QeVector4f vec4 = QeVector4f(vec, 1.0f);
	pos = mat * vec4;

	/*if (polarAngle < 0.1f && _angle < 0) return;
	if (polarAngle > 179.9f && _angle > 0) return;
	
	if (_axis.x) {
		polarAngle += _angle;

		if (polarAngle > 180) {
			polarAngle = 179.99f;
		}
		else if (polarAngle < 0) {
			polarAngle = 0.01f;
		}
	}
	else azimuthalAngle += _angle;

	while (polarAngle > 360) polarAngle -= 360;
	while (polarAngle < -360) polarAngle += 360;
	while (azimuthalAngle > 360) azimuthalAngle -= 360;
	while (azimuthalAngle < -360) azimuthalAngle += 360;

	MATH->rotatefromCenter(center, pos, polarAngle, azimuthalAngle);*/
	face = MATH->normalize(center - pos);
	bUpdate = true;
}

/*void QeCamera::rotatePos(QeVector2i mousePos){

	//if (type == eCameraThirdPerson) return;

	rotatePos(float(mousePos.x - lastMousePos.x), QeVector3f(0, 0, 1));
	rotatePos(float(mousePos.y - lastMousePos.y), QeVector3f(1, 0, 0));
	lastMousePos = mousePos;
}*/

void QeCamera::rotateTarget(QeVector2i mousePos){

	rotateTarget(float(mousePos.x - lastMousePos.x), { 0.0f, 1.0f });
	rotateTarget(float(mousePos.y - lastMousePos.y), { 1.0f, 0.0f });
	lastMousePos = mousePos;
}

void QeCamera::setMousePos(QeVector2i mousePos) {
	lastMousePos = mousePos;
}

void QeCamera::zoomInOut(QeVector2i mousePos) {
	
	move( { 0, 0, -(mousePos.y - lastMousePos.y) / 10 }, false);
	lastMousePos = mousePos;
}

void QeCamera::move(QeVector3f _dir, bool bMoveCenter) {

	QeVector3f move;
	// forward
	if (_dir.z) {
		if (_dir.z > 0 && MATH->length(pos - center) < 1) {
			return;
		}
		move = face * _dir.z;
	}
	else {
		QeVector3f _surface = MATH->normalize(MATH->cross(face, up));
		//left
		if (_dir.x) {
			move = _surface * _dir.x;
		}
		//up
		if (_dir.y) {
			QeVector3f _up1 = MATH->cross(_surface, face);
			move = _up1 * _dir.y;
		}
	}
	move *= speed;
	pos += move;

	//if (type == eCameraFirstPerson) {
	if(bMoveCenter)		center += move;
	face = MATH->normalize(center - pos);
	bUpdate = true;
}

void QeCamera::reset() {
	QePoint::reset();
	updateAxis();
}

void QeCamera::setMatrix() {
	bufferData.view = MATH->lookAt(pos, center, up);
	bufferData.projection = MATH->perspective(fov, faspect, fnear, ffar);
	bufferData.pos = pos;
}


void QeCamera::updateCompute(float time) {

	if (bUpdate) {
		updateAxis();
		setMatrix();
	}
}

void QeCamera::updateAxis() {
	if(ACT && ACT->axis)	ACT->axis->pos = center;
}