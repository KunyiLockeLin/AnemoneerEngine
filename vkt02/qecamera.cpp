#include "qecamera.h"


void QeCamera::setCamera(QeVector3f _pos, QeVector3f _target, QeVector3f _up, float _fov, float _near, float _far) {
	pos = _pos;
	target = _target;
	up = _up;
	fov = _fov;
	fnear = _near;
	ffar = _far;
}

void QeCamera::rotatePos(float _angle, QeVector3f _axis) {

	if (type == eCameraThirdPerson) return;

	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = target - pos;
	QeMatrix4x4f mat = MATH->rotate(_angle*rotateSpeed, _axis);
	mat *= MATH->translate(pos);
	QeVector4f v4(vec, 1);
	target = mat*v4;
}
void QeCamera::rotateTarget(float _angle, QeVector3f _axis) {
	
	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = pos - target;
	QeMatrix4x4f mat = MATH->rotate(_angle*rotateSpeed, _axis); 
	mat *= MATH->translate(target);
	QeVector4f v4(vec, 1);
	pos = mat*v4;
}

void QeCamera::rotatePos(QeVector2i mousePos){

	if (type == eCameraThirdPerson) return;

	rotatePos(float(mousePos.x - lastMousePos.x), QeVector3f(0, 0, 1));
	rotatePos(float(mousePos.y - lastMousePos.y), QeVector3f(1, 0, 0));
	lastMousePos = mousePos;
}

void QeCamera::rotateTarget(QeVector2i mousePos){

	rotateTarget(float(mousePos.x - lastMousePos.x), QeVector3f(0, 0, 1));
	rotateTarget(float(mousePos.y - lastMousePos.y), QeVector3f(1, 0, 0));
	lastMousePos = mousePos;
}
void QeCamera::setMousePos(QeVector2i mousePos) {
	lastMousePos = mousePos;
}
void QeCamera::move(QeVector3f _dir) {

	QeMatrix4x4f mat;

	QeVector3f _face = MATH->normalize(target - pos);

	// forward
	if (_dir.z != 0) {
		mat = MATH->translate(_face*_dir.z*moveSpeed);
	}

	QeVector3f _surface = MATH->normalize(MATH->cross(_face, up));
	//left
	if (_dir.x != 0) {
		mat = MATH->translate(_surface*_dir.x*moveSpeed);
	}
	//up
	if (_dir.y != 0) {
		QeVector3f _up1 = MATH->cross(_surface, _face);
		mat = MATH->translate(_up1*_dir.y*moveSpeed);
	}

	QeVector4f v4(pos, 1);
	v4 = mat*v4;
	pos = v4;

	if (type == eCameraFirstPerson) {
		v4 = target;
		v4 = mat*v4;
		target = v4;
	}
}

void QeCamera::reset() {
	pos = QeVector3f(5, 5, 5);
	target = QeVector3f(0, 0, 0);
	up = QeVector3f(0, 0, 1);
	fov = 45.0f;
	fnear = 0.1f;
	ffar = 1000.0f;
	type = eCameraFirstPerson;
	//type = eCameraThirdPerson;
}

void QeCamera::update(float time) {

	//rotatePos(timec,  QeVector3f(0,0,1) );
	//rotateTarget(timec, QeVector3f(0, 0, 1));
}

void QeCamera::switchType(QeCameraType _type) {
	type = _type;
}

QeMatrix4x4f QeCamera::getMatView() {
	return MATH->lookAt( pos, target, up );
}

QeMatrix4x4f QeCamera::getMatProjection() {

	int width, height;
	QE->window->getWindowSize(width, height);
	
	QeMatrix4x4f _proj = MATH->perspective( fov, float(width)/height, fnear, ffar);
	return _proj;
}