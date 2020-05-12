#include "header.h"

void QeTransform::initialize(AeXMLNode *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);
    _owner->transform = this;

    position = initProperty->getXMLValuefXYZ("position");
    scale = initProperty->getXMLValuefXYZ("scale");
    faceEular = initProperty->getXMLValuefXYZ("faceEular");
    rotateSpeed = initProperty->getXMLValuefXYZ("rotateSpeed");
    revoluteSpeed = initProperty->getXMLValuefXYZ("revoluteSpeed");
    revoluteFixAxisX = initProperty->getXMLValueb("revoluteFixAxisX");
    revoluteFixAxisY = initProperty->getXMLValueb("revoluteFixAxisY");
    revoluteFixAxisZ = initProperty->getXMLValueb("revoluteFixAxisZ");
    targetAnimationOID = initProperty->getXMLValuei("targetAnimationOID");
    targetBoneName = initProperty->getXMLValue("targetBoneName");
}

void QeTransform::update1() {
    if (rotateSpeed.x != 0.f || rotateSpeed.y != 0.f || rotateSpeed.z != 0.f) {
        faceEular += (rotateSpeed * ENGINE->deltaTime * 30.f);
    }

    if ((owner && owner->owner && owner->owner->transform) &&
        (revoluteSpeed.x != 0.f || revoluteSpeed.y != 0.f || revoluteSpeed.z != 0.f)) {
        revolute((revoluteSpeed * ENGINE->deltaTime * 30.f), owner->owner->transform->worldPosition(), revoluteFixAxisX,
                 revoluteFixAxisY, revoluteFixAxisZ);
    }
}

QeVector3f QeTransform::worldPosition() {
    if (targetAnimationOID) {
        QeAnimation *animation = (QeAnimation *)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
        if (animation) {
            QeVector4f vec = {position, 1};
            return animation->getBoneTransfrom(targetBoneName) * vec;
        }
    }

    QeVector3f _ret = position;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->position;
        _parent = _parent->owner;
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
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret *= _parent->transform->scale;
        _parent = _parent->owner;
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
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->faceEular;
        _parent = _parent->owner;
    }
    return _ret;
}

QeVector3f QeTransform::worldFaceVector() { return MATH->eulerAnglesToVector(worldFaceEular()); }

QeVector3f QeTransform::localFaceVector() { return MATH->eulerAnglesToVector(faceEular); }

void QeTransform::setWorldPosition(QeVector3f &_worldPosition) {
    position = _worldPosition;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) position -= _parent->transform->position;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldScale(QeVector3f &_worldScale) {
    scale = _worldScale;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) scale /= _parent->transform->scale;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldFaceByEular(QeVector3f &_worldFace) {
    faceEular = _worldFace;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) faceEular -= _parent->transform->faceEular;
        _parent = _parent->owner;
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
        QeAnimation *animation = (QeAnimation *)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
        if (animation) {
            return animation->getBoneTransfrom(targetBoneName) *
                   MATH->getTransformMatrix(position, faceEular, scale,
                                            GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
        }
    }

    return MATH->getTransformMatrix(worldPosition(), worldFaceEular(), worldScale(),
                                    GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
}
