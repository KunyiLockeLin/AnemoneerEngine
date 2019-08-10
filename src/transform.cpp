#include "header.h"

void QeTransform::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);
    _owner->transform = this;

    AST->getXMLfValue(&position.x, initProperty, 1, "positionX");
    AST->getXMLfValue(&position.y, initProperty, 1, "positionY");
    AST->getXMLfValue(&position.z, initProperty, 1, "positionZ");
    AST->getXMLfValue(&scale.x, initProperty, 1, "scaleX");
    AST->getXMLfValue(&scale.y, initProperty, 1, "scaleY");
    AST->getXMLfValue(&scale.z, initProperty, 1, "scaleZ");
    AST->getXMLfValue(&faceEular.x, initProperty, 1, "faceEularX");
    AST->getXMLfValue(&faceEular.y, initProperty, 1, "faceEularY");
    AST->getXMLfValue(&faceEular.z, initProperty, 1, "faceEularZ");
    AST->getXMLfValue(&rotateSpeed.x, initProperty, 1, "rotateSpeedX");
    AST->getXMLfValue(&rotateSpeed.y, initProperty, 1, "rotateSpeedY");
    AST->getXMLfValue(&rotateSpeed.z, initProperty, 1, "rotateSpeedZ");
    AST->getXMLfValue(&revoluteSpeed.x, initProperty, 1, "revoluteSpeedX");
    AST->getXMLfValue(&revoluteSpeed.y, initProperty, 1, "revoluteSpeedY");
    AST->getXMLfValue(&revoluteSpeed.z, initProperty, 1, "revoluteSpeedZ");
    AST->getXMLbValue(&revoluteFixAxisX, initProperty, 1, "revoluteFixAxisX");
    AST->getXMLbValue(&revoluteFixAxisY, initProperty, 1, "revoluteFixAxisY");
    AST->getXMLbValue(&revoluteFixAxisZ, initProperty, 1, "revoluteFixAxisZ");
    targetAnimationOID = 0;
    AST->getXMLiValue(&targetAnimationOID, initProperty, 1, "targetAnimationOID");
    targetBoneName = AST->getXMLValue(initProperty, 1, "targetBoneName");
}

void QeTransform::update1() {
    if (rotateSpeed.x != 0.f || rotateSpeed.y != 0.f || rotateSpeed.z != 0.f) {
        faceEular += (rotateSpeed * QE->deltaTime * 30.f);
    }

    if ((owner && owner->parent && owner->parent->transform) &&
        (revoluteSpeed.x != 0.f || revoluteSpeed.y != 0.f || revoluteSpeed.z != 0.f)) {
        revolute((revoluteSpeed * QE->deltaTime * 30.f), owner->parent->transform->worldPosition(), revoluteFixAxisX,
                 revoluteFixAxisY, revoluteFixAxisZ);
    }
}

QeVector3f QeTransform::worldPosition() {
    if (targetAnimationOID) {
        QeAnimation *animation = (QeAnimation *)SCENE->findComponent(eComponent_animation, targetAnimationOID);
        if (animation) {
            QeVector4f vec = {position, 1};
            return animation->getBoneTransfrom(targetBoneName) * vec;
        }
    }

    QeVector3f _ret = position;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->position;
        _parent = _parent->parent;
    }
    return _ret;
}

QeVector3f QeTransform::worldScale() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return scale * animation->getBoneScale(targetBoneName);
            }
    }*/

    QeVector3f _ret = scale;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) _ret *= _parent->transform->scale;
        _parent = _parent->parent;
    }
    return _ret;
}

QeVector3f QeTransform::worldFaceEular() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return faceEular +
    animation->getBoneRotateEuler(targetBoneName);
            }
    }*/

    QeVector3f _ret = faceEular;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->faceEular;
        _parent = _parent->parent;
    }
    return _ret;
}

QeVector3f QeTransform::worldFaceVector() { return MATH->eulerAnglesToVector(worldFaceEular()); }

QeVector3f QeTransform::localFaceVector() { return MATH->eulerAnglesToVector(faceEular); }

void QeTransform::setWorldPosition(QeVector3f &_worldPosition) {
    position = _worldPosition;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) position -= _parent->transform->position;
        _parent = _parent->parent;
    }
}

void QeTransform::setWorldScale(QeVector3f &_worldScale) {
    scale = _worldScale;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) scale /= _parent->transform->scale;
        _parent = _parent->parent;
    }
}

void QeTransform::setWorldFaceByEular(QeVector3f &_worldFace) {
    faceEular = _worldFace;
    QeObject *_parent = owner->parent;

    while (_parent) {
        if (_parent->transform) faceEular -= _parent->transform->faceEular;
        _parent = _parent->parent;
    }
}

void QeTransform::setWorldFaceByVector(QeVector3f &_worldFaceVector) {
    setWorldFaceByEular(MATH->vectorToEulerAngles(_worldFaceVector));
}

void QeTransform::move(QeVector3f &_addMove, QeVector3f &_face, QeVector3f &_up) {
    position = MATH->move(position, _addMove, _face, _up);
}

void QeTransform::revolute(QeVector3f &_addRevolute, QeVector3f &_centerPosition, bool bFixX, bool bFixY, bool bFixZ) {
    setWorldPosition(MATH->revolute_axis(worldPosition(), _addRevolute, _centerPosition, bFixX, bFixY, bFixZ));
}

QeMatrix4x4f QeTransform::worldTransformMatrix(bool bRotate, bool bFixSize) {
    if (targetAnimationOID) {
        QeAnimation *animation = (QeAnimation *)SCENE->findComponent(eComponent_animation, targetAnimationOID);
        if (animation) {
            return animation->getBoneTransfrom(targetBoneName) *
                   MATH->getTransformMatrix(position, faceEular, scale, bRotate, bFixSize);
        }
    }

    return MATH->getTransformMatrix(worldPosition(), worldFaceEular(), worldScale(), bRotate, bFixSize);
}
