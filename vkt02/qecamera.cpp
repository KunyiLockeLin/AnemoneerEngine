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

	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = target - pos;
	QeMatrix4x4f mat = MATH->translate(pos);
	mat *= MATH->rotate(_angle*rotateSpeed, _axis);
	QeVector4f v4(vec, 1);
	target = mat*v4;
}
void QeCamera::rotateTarget(float _angle, QeVector3f _axis) {
	
	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = pos - target;
	QeMatrix4x4f mat = MATH->translate(target);
	mat *= MATH->rotate(_angle*rotateSpeed, _axis);
	QeVector4f v4(vec, 1);
	pos = mat*v4;
}

void QeCamera::rotatePos(QeVector2i mousePos){

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

	if (bFirstPerson) {
		v4 = target;
		v4 = mat*v4;
		target = v4;
	}
}

void QeCamera::reset() {
	pos = QeVector3f(0, 3, 0);
	target = QeVector3f(0, 0, 0);
	up = QeVector3f(0, 0, 1);
	fov = 45.0f;
	fnear = 0.1f;
	ffar = 1000.0f;
	bFirstPerson = !bFirstPerson;
}

void QeCamera::update() {

	//static auto startTimec = std::chrono::high_resolution_clock::now();
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float timec = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTimec).count() / 1000.0f;
	//startTimec = currentTime;
	//static float timec = 0;
	//timec = 0.01f;
	//rotatePos(timec,  QeVector3f(0,0,1) );
}

void QeCamera::setFirstPerson(bool _bFirstPerson) {
	bFirstPerson = _bFirstPerson;
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