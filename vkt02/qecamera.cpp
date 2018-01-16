#include "qeheader.h"


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
	QeMatrix4x4f mat = MATH->rotate(_angle*speed, _axis);
	mat *= MATH->translate(pos);
	QeVector4f v4(vec, 1);
	target = mat*v4;
}
void QeCamera::rotateTarget(float _angle, QeVector3f _axis) {
	
	while (_angle > 360) _angle -= 360;
	while (_angle < -360) _angle += 360;

	QeVector3f vec = pos - target;
	QeMatrix4x4f mat = MATH->rotate(_angle*speed, _axis);
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
		mat = MATH->translate(_face*_dir.z*speed);
	}

	QeVector3f _surface = MATH->normalize(MATH->cross(_face, up));
	//left
	if (_dir.x != 0) {
		mat = MATH->translate(_surface*_dir.x*speed);
	}
	//up
	if (_dir.y != 0) {
		QeVector3f _up1 = MATH->cross(_surface, _face);
		mat = MATH->translate(_up1*_dir.y*speed);
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

void QeCamera::init(QeAssetXML* _property) {

	pos = QeVector3f(0, 10, 5);
	target = QeVector3f(0, 0, 0);
	up = QeVector3f(0, 0, 1);
	fov = 45.0f;
	fnear = 0.1f;
	ffar = 1000.0f;
	type = eCameraThirdPerson;
	speed = 0.5f;

	if (_property == nullptr) return;

	initProperty = _property;
	const char* c;
	
	c = AST->getXMLValue(_property, 1, "type");
	if (c != nullptr)	type = QeCameraType(atoi(c));
	else				type = eCameraFirstPerson;

	c = AST->getXMLValue(_property, 1, "posX");
	if (c != nullptr)	pos.x = float(atof(c));

	c = AST->getXMLValue(_property, 1, "posY");
	if (c != nullptr)	pos.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "posZ");
	if (c != nullptr)	pos.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "targetX");
	if (c != nullptr)	target.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "targetY");
	if (c != nullptr)	target.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "targetZ");
	if (c != nullptr)	target.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "upX");
	if (c != nullptr)	up.x = float(atof(c));
	c = AST->getXMLValue(_property, 1, "upY");
	if (c != nullptr)	up.y = float(atof(c));
	c = AST->getXMLValue(_property, 1, "upZ");
	if (c != nullptr)	up.z = float(atof(c));

	c = AST->getXMLValue(_property, 1, "speed");
	if (c != nullptr)	speed = float(atof(c));


	c = AST->getXMLValue(_property, 1, "fov");
	if (c != nullptr)	fov = float(atof(c));
	c = AST->getXMLValue(_property, 1, "near");
	if (c != nullptr)	fnear = float(atof(c));
	c = AST->getXMLValue(_property, 1, "far");
	if (c != nullptr)	ffar = float(atof(c));
}

void QeCamera::reset() {

	QeCameraType lastType = type;

	init(initProperty); 

	if (lastType == eCameraThirdPerson) type = eCameraFirstPerson;
	else								type = eCameraFirstPerson;
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

	QeMatrix4x4f _proj = MATH->perspective( fov, faspect, fnear, ffar);
	return _proj;
}