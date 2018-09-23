#include "qeheader.h"

void QeCamera::initialize(QeAssetXML* _property, QeObject* _owner) {
	
	QeComponent::initialize(_property, _owner);
	//type = eCameraThirdPerson;

	AST->getXMLiValue(&lookAtTransformOID, initProperty, 1, "lookAtTransformOID");
	AST->getXMLbValue(&bMain, initProperty, 1, "main");
	AST->getXMLfValue(&up.x, initProperty, 1, "upX");
	AST->getXMLfValue(&up.y, initProperty, 1, "upY");
	AST->getXMLfValue(&up.z, initProperty, 1, "upZ");
	AST->getXMLfValue(&speed, initProperty, 1, "speed");
	AST->getXMLiValue(&cullingDistance, initProperty, 1, "cullingDistance");
	AST->getXMLfValue(&fov, initProperty, 1, "fov");
	AST->getXMLfValue(&fnear, initProperty, 1, "near");
	AST->getXMLfValue(&ffar, initProperty, 1, "far");

	if (bMain) {
		GRAP->createRender(eRender_main, oid, { 0,0 });
	}

	bUpdate = true;
}

void QeCamera::rotateTarget(QeVector2f _addRotate) {

	QeVector3f lookAtV = lookAt();
	QeVector3f pos = owner->transform->worldPosition();
	QeVector3f vec = { pos - lookAtV };

	QeVector3f euler = MATH->vectorToEulerAngles(vec);
	if ((euler.y < -85 && _addRotate.x < 0) || (euler.y > 85 && _addRotate.x > 0)) return;
	_addRotate *= speed;

	if (_addRotate.x) {
		float f = euler.y + _addRotate.x;
		if (f < -85)		_addRotate.x = -89-euler.y;
		else if (f > 85)	_addRotate.x = 89-euler.y;
	}
	owner->transform->revolute(_addRotate, lookAtV);
	bUpdate = true;
}

void QeCamera::rotateTargetByMouse(QeVector2i mousePos){

	rotateTarget({ float(mousePos.y - lastMousePos.y), float(mousePos.x - lastMousePos.x) });
	lastMousePos = mousePos;
}

void QeCamera::setMousePos(QeVector2i mousePos) {
	lastMousePos = mousePos;
}

void QeCamera::zoomInOut(QeVector2i mousePos) {
	
	move( { 0, 0, -(mousePos.x - lastMousePos.x) / 10 }, false);
	lastMousePos = mousePos;
}

void QeCamera::move(QeVector3f _dir, bool bMoveCenter) {

	QeVector3f lookAtV = lookAt();
	QeVector3f pos = owner->transform->worldPosition();
	QeVector3f face = pos - lookAtV;

	// forward
	if (_dir.z > 0 && !bMoveCenter && MATH->length(face) < 1) {
		return;
	}
	_dir *= speed;
	owner->transform->move(_dir, face, up);
	pos = owner->transform->worldPosition();

	if (bMoveCenter) {
		QeTransform* lookAtTransform = (QeTransform*)OBJMGR->findComponent(eComponent_transform, lookAtTransformOID);
		lookAtTransform->move(_dir, face, up);
		owner->transform->setWorldPosition(pos);
	}

	bUpdate = true;
}

QeVector3f QeCamera::face() {	return 	MATH->normalize(lookAt() - owner->transform->worldPosition()); }

QeVector3f QeCamera::lookAt() {
	if (lookAtTransformOID>0) {
		QeTransform* lookAtTransform = (QeTransform*)OBJMGR->findComponent(eComponent_transform, lookAtTransformOID);
		return lookAtTransform->worldPosition();
	}
	return { 0.f, 0.f, 0.f };
}

void QeCamera::reset() {

	if (lookAtTransformOID>0) {
		QeTransform* lookAtTransform = (QeTransform*)OBJMGR->findComponent(eComponent_transform, lookAtTransformOID);
		lookAtTransform->initialize(lookAtTransform->owner->transform->initProperty, lookAtTransform->owner);
	}
	owner->transform->initialize(owner->transform->initProperty, owner);
	initialize(initProperty, owner);
}

void QeCamera::update1() {

	//if (bUpdate) {
		QeVector3f lookAtV = lookAt();
		QeVector3f pos = owner->transform->worldPosition();

		bufferData.view = MATH->lookAt(pos, lookAtV, up);
		bufferData.projection = MATH->perspective(fov, faspect, fnear, ffar);
		bufferData.pos = pos;
		bUpdate = false;
	//}
}