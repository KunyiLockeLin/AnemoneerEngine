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

	AST->getXMLiValue(&postProcessingOID, initProperty, 1, "postProcessingOID");
	bUpdatePostProcessingOID = false;
	if (postProcessingOID) bUpdatePostProcessingOID = true;
}

void QeCamera::rotateTarget(QeVector3f _addRotate) {

	QeVector3f lookAtV = lookAt();
	QeVector3f pos = owner->transform->worldPosition();
	QeVector3f vec = { pos - lookAtV };

	QeVector3f euler = MATH->vectorToEulerAngles(vec);
	if ((euler.y < -85 && _addRotate.y < 0) || (euler.y > 85 && _addRotate.y > 0)) return;
	_addRotate *= speed;

	if (_addRotate.x) {
		float f = euler.y + _addRotate.y;
		if (f < -85)		_addRotate.y = -89-euler.y;
		else if (f > 85)	_addRotate.y = 89-euler.y;
	}
	owner->transform->revolute(_addRotate, lookAtV, false, false, true);
}

void QeCamera::rotateTargetByMouse(QeVector2i mousePos){

	rotateTarget({0.f, float(mousePos.y - lastMousePos.y), float(mousePos.x - lastMousePos.x) });
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

void QeCamera::setLookAtTransformOID(int _lookAtransformOID) {
	if (!lookAtTransformOID) return;
	if (!_lookAtransformOID) return;

	QeTransform* lookAtTransform1 = (QeTransform*)OBJMGR->findComponent(eComponent_transform, _lookAtransformOID);
	if (!lookAtTransform1) return;
	QeTransform* lookAtTransform2 = (QeTransform*)OBJMGR->findComponent(eComponent_transform, lookAtTransformOID);
	if (!lookAtTransform2) return;

	lookAtTransform2->setWorldPosition(lookAtTransform1->worldPosition());
	owner->transform->localPosition = { 20.f,0.f,1.f };
}

void QeCamera::update1() {

	if (bUpdatePostProcessingOID && postProcessingOID) {
		QeRenderType type = eRender_main;
		if (!bMain) type = eRender_color;

		if (GRAP->addPostProcssing(type, oid, postProcessingOID)) {
			bUpdatePostProcessingOID = false;
		}
	}

	QeVector3f lookAtV = lookAt();
	QeVector3f pos = owner->transform->worldPosition();

	bufferData.view = MATH->lookAt(pos, lookAtV, up);
	bufferData.projection = MATH->perspective(fov, faspect, fnear, ffar);
	bufferData.pos = pos;
}