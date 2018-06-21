#include "qeheader.h"


void QeCamera::setProperty() {
	QePoint::setProperty();
	//type = eCameraThirdPerson;

	center = { 0.0f, 0.0f, 0.0f };
	AST->getXMLfValue(&center.x, initProperty, 1, "centerX");
	AST->getXMLfValue(&center.y, initProperty, 1, "centerY");
	AST->getXMLfValue(&center.z, initProperty, 1, "centerZ");
	face = MATH->normalize(center - pos);

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


void QeCamera::setCamera(QeVector3f& _pos, QeVector3f& _center, QeVector3f& _up, float _fov, float _near, float _far) {
	pos = _pos;
	center = _center;
	up = _up;
	fov = _fov;
	fnear = _near;
	ffar = _far;
	face = MATH->normalize(center - pos);
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

	QeVector3f vec = pos - center;

	if (_axis.y) {
		
		float outPolarAngle, outAzimuthalAngle;
		MATH->getAnglefromVector(vec, outPolarAngle, outAzimuthalAngle);
		float len = MATH->length(vec);
		if (_angle > 0) {
			if (outPolarAngle > 89) return;
			float f = outPolarAngle + _angle;
			if (f > 90) _angle = 90 - outPolarAngle;
		} 
		else if (_angle < 0) {
			if (outPolarAngle < -89) return;
			float f = outPolarAngle + _angle;
			if (f < -90) _angle = -90 - outPolarAngle;
		}
		_axis = MATH->cross(vec, up);
	}

	QeMatrix4x4f mat;
	mat *= MATH->rotate(_angle*speed, _axis);
	mat *= MATH->translate(center);
	QeVector4f v4(vec, 1);
	pos = mat*v4;
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

	rotateTarget(float(mousePos.x - lastMousePos.x), { 0.0f, 0.0f, 1.0f });
	rotateTarget(-float(mousePos.y - lastMousePos.y), { 0.0f, 1.0f, 0.0f });
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

	QeVector3f _face = MATH->normalize(center - pos);

	// forward
	if (_dir.z) {
		if (_dir.z >0 && MATH->length(pos - center) < 1) return;
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
		v4 = center;
		v4 = mat*v4;
		center = v4;
	}
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