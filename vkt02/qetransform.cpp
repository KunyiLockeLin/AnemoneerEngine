#include "qeheader.h"

void QeTransform::initialize(QeAssetXML* _property, QeObject* _owner) {
	
	QeComponent::initialize(_property, _owner);
	_owner->transform = this;

	AST->getXMLfValue(&localPosition.x, initProperty, 1, "localPositionX");
	AST->getXMLfValue(&localPosition.y, initProperty, 1, "localPositionY");
	AST->getXMLfValue(&localPosition.z, initProperty, 1, "localPositionZ");
	AST->getXMLfValue(&localScale.x, initProperty, 1, "localScaleX");
	AST->getXMLfValue(&localScale.y, initProperty, 1, "localScaleY");
	AST->getXMLfValue(&localScale.z, initProperty, 1, "localScaleZ");
	AST->getXMLfValue(&localFaceEular.x, initProperty, 1, "localFaceEularX");
	AST->getXMLfValue(&localFaceEular.y, initProperty, 1, "localFaceEularY");
	AST->getXMLfValue(&localFaceEular.z, initProperty, 1, "localFaceEularZ");
	AST->getXMLfValue(&rotateSpeed.x, initProperty, 1, "rotateSpeedX");
	AST->getXMLfValue(&rotateSpeed.y, initProperty, 1, "rotateSpeedY");
	AST->getXMLfValue(&rotateSpeed.z, initProperty, 1, "rotateSpeedZ");
	AST->getXMLfValue(&revoluteSpeed.x, initProperty, 1, "revoluteSpeedX");
	AST->getXMLfValue(&revoluteSpeed.y, initProperty, 1, "revoluteSpeedY");
	AST->getXMLfValue(&revoluteSpeed.z, initProperty, 1, "revoluteSpeedZ");
	targetAnimationOID = 0;
	AST->getXMLiValue(&targetAnimationOID, initProperty, 1, "targetAnimationOID");
	targetBoneName = AST->getXMLValue(initProperty, 1, "targetBoneName");
}

void QeTransform::update1() {
	if (rotateSpeed.x != 0.f || rotateSpeed.x != 0.f || rotateSpeed.z != 0.f) {
		localFaceEular += (rotateSpeed*QE->deltaTime*30.f);
	}

	if ( (owner && owner->parent && owner->parent->transform) &&
		(revoluteSpeed.x != 0.f || revoluteSpeed.y != 0.f || revoluteSpeed.z != 0.f )) {
		revolute((revoluteSpeed*QE->deltaTime*30.f), owner->parent->transform->worldPosition());
	}
}


QeVector3f QeTransform::worldPosition() {
	
	if (targetAnimationOID) {
		QeAnimation* animation = (QeAnimation*)OBJMGR->findComponent( eComponent_animation, targetAnimationOID);
		if (animation) {
			QeVector4f vec = { localPosition, 1 };
			return animation->getBoneTransfrom(targetBoneName)* vec;
		}
	}

	QeVector3f _ret = localPosition;
	QeObject * _parent = owner->parent;
	
	while (_parent ) {
		if (_parent->transform)	_ret += _parent->transform->localPosition;
		_parent = _parent->parent;
	}
	return _ret;
}

QeVector3f QeTransform::worldScale() {

	/*if (targetAnimationOID) {
		QeAnimation* animation = (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
		if (animation) {
			return localScale * animation->getBoneScale(targetBoneName);
		}
	}*/

	QeVector3f _ret = localScale;
	QeObject * _parent = owner->parent;

	while (_parent) {
		if (_parent->transform)	_ret *= _parent->transform->localScale;
		_parent = _parent->parent;
	}
	return _ret;
}

QeVector3f QeTransform::worldFaceEular() {

	/*if (targetAnimationOID) {
		QeAnimation* animation = (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
		if (animation) {
			return localFaceEular + animation->getBoneRotateEuler(targetBoneName);
		}
	}*/

	QeVector3f _ret = localFaceEular;
	QeObject * _parent = owner->parent;

	while (_parent) {
		if (_parent->transform)	_ret += _parent->transform->localFaceEular;
		_parent = _parent->parent;
	}
	return _ret;
}

QeVector3f QeTransform::worldFaceVector() {
	return MATH->eulerAnglesToVector(worldFaceEular());
}

QeVector3f QeTransform::localFaceVector() {
	return MATH->eulerAnglesToVector(localFaceEular);
}

void QeTransform::setWorldPosition(QeVector3f& _worldPosition) {

	localPosition = _worldPosition;
	QeObject * _parent = owner->parent;

	while (_parent) {
		if (_parent->transform)	localPosition -= _parent->transform->localPosition;
		_parent = _parent->parent;
	}
}

void QeTransform::setWorldScale(QeVector3f& _worldScale) {

	localScale = _worldScale;
	QeObject * _parent = owner->parent;

	while (_parent) {
		if (_parent->transform)	localScale /= _parent->transform->localScale;
		_parent = _parent->parent;
	}
}

void QeTransform::setWorldFaceByEular(QeVector3f& _worldFace) {

	localFaceEular = _worldFace;
	QeObject * _parent = owner->parent;

	while (_parent) {
		if (_parent->transform)	localFaceEular -= _parent->transform->localFaceEular;
		_parent = _parent->parent;
	}
}

void QeTransform::setWorldFaceByVector(QeVector3f& _worldFaceVector) {
	setWorldFaceByEular(MATH->vectorToEulerAngles(_worldFaceVector));
}

void QeTransform::move(QeVector3f& _addMove, QeVector3f& _face, QeVector3f& _up) {
	
	localPosition = MATH->move(localPosition, _addMove, _face, _up);
}

void QeTransform::revolute(QeVector3f& _addRevolute, QeVector3f& _centerPosition, bool bFixX, bool bFixY, bool bFixZ) {
	setWorldPosition( MATH->revolute(worldPosition(), _addRevolute, _centerPosition, bFixX, bFixY, bFixZ) );
}

QeMatrix4x4f QeTransform::worldTransformMatrix(bool bRotate, bool bFixSize ) {

	if (targetAnimationOID) {
		QeAnimation* animation = (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
		if (animation) {
			return animation->getBoneTransfrom(targetBoneName) * MATH->getTransformMatrix(localPosition, localFaceEular, localScale, bRotate, bFixSize);
		}
	}

	return MATH->getTransformMatrix(worldPosition(), worldFaceEular(), worldScale(), bRotate, bFixSize);
}