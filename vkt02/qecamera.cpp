#include "qeheader.h"


void QeCamera::setProperty() {
	QePoint::setProperty();
	//type = eCameraThirdPerson;

	target = { 0.0f, 0.0f, 0.0f };
	AST->getXMLfValue(&target.x, initProperty, 1, "targetX");
	AST->getXMLfValue(&target.y, initProperty, 1, "targety");
	AST->getXMLfValue(&target.z, initProperty, 1, "targetz");
	face = MATH->normalize(target - pos);

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
}


void QeCamera::setCamera(QeVector3f& _pos, QeVector3f& _target, QeVector3f& _up, float _fov, float _near, float _far) {
	pos = _pos;
	target = _target;
	up = _up;
	fov = _fov;
	fnear = _near;
	ffar = _far;
	face = MATH->normalize(target - pos);
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

void QeCamera::rotateTarget(float _angle, QeVector3f _axis) {
	
	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = pos - target;
	QeMatrix4x4f mat = MATH->rotate(_angle*speed, _axis);
	mat *= MATH->translate(target);
	QeVector4f v4(vec, 1);
	pos = mat*v4;
	face = MATH->normalize(target - pos);
	bUpdate = true;
}

/*void QeCamera::rotatePos(QeVector2i mousePos){

	//if (type == eCameraThirdPerson) return;

	rotatePos(float(mousePos.x - lastMousePos.x), QeVector3f(0, 0, 1));
	rotatePos(float(mousePos.y - lastMousePos.y), QeVector3f(1, 0, 0));
	lastMousePos = mousePos;
}*/

void QeCamera::rotateTarget(QeVector2i mousePos){

	rotateTarget(float(mousePos.x - lastMousePos.x), { 0.0f, 0.0f, 1.0f });
	rotateTarget(float(mousePos.y - lastMousePos.y), { 0.0f, 1.0f, 0.0f });
	lastMousePos = mousePos;
}

void QeCamera::setMousePos(QeVector2i mousePos) {
	lastMousePos = mousePos;
}

void QeCamera::zoomInOut(QeVector2i mousePos) {
	
	move( { 0, 0, -(mousePos.y - lastMousePos.y) / 10 }, false);
	lastMousePos = mousePos;
}

void QeCamera::move(QeVector3f _dir, bool bMoveTarget) {

	QeMatrix4x4f mat;

	QeVector3f _face = MATH->normalize(target - pos);

	// forward
	if (_dir.z) {
		mat = MATH->translate(_face*_dir.z*speed);
	}
	else {
		QeVector3f _surface = MATH->normalize(MATH->cross(_face, up));
		//left
		if (_dir.x) {
			mat = MATH->translate(_surface*_dir.x*speed);
		}
		//up
		if (_dir.y) {
			QeVector3f _up1 = MATH->cross(_surface, _face);
			mat = MATH->translate(_up1*_dir.y*speed);
		}
	}
	QeVector4f v4(pos, 1);
	v4 = mat*v4;
	pos = v4;

	//if (type == eCameraFirstPerson) {
	if(bMoveTarget) {
		v4 = target;
		v4 = mat*v4;
		target = v4;
	}
	face = MATH->normalize(target - pos);
	bUpdate = true;
}

void QeCamera::reset() {
	QePoint::reset();
	updateAxis();
}

void QeCamera::setMatrix() {
	bufferData.view = MATH->lookAt(pos, target, up);
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
	if(ACT && ACT->axis)	ACT->axis->pos = target;
}