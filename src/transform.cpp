#include "header.h"

void QeTransform::initialize(AeXMLNode *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);
    data.read(*_property);
    _owner->transform = this;
}

void QeTransform::updatePreRender() {
    if (data.rotateSpeed.x != 0.f || data.rotateSpeed.y != 0.f || data.rotateSpeed.z != 0.f) {
        data.faceEular += (data.rotateSpeed * ENGINE->deltaTime * 30.f);
    }

    if ((owner && owner->owner && owner->owner->transform) &&
        (data.revoluteSpeed.x != 0.f || data.revoluteSpeed.y != 0.f || data.revoluteSpeed.z != 0.f)) {
        revolute((data.revoluteSpeed * ENGINE->deltaTime * 30.f), owner->owner->transform->worldPosition(), data.revoluteFixAxis.x,
                 data.revoluteFixAxis.y, data.revoluteFixAxis.y);
    }
}

AeVector<float, 3> QeTransform::worldPosition() {
    if (data.targetAnimationOID) {
        QeAnimation *animation = (QeAnimation *)OBJMGR->findComponent(eGAMEOBJECT_Component_Animation, data.targetAnimationOID);
        if (animation) {
            AeVector<float, 4> vec = {data.position, 1};
            return animation->getBoneTransfrom(data.targetBoneName.c_str()) * vec;
        }
    }

    AeVector<float, 3> _ret = data.position;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->data.position;
        _parent = _parent->owner;
    }
    return _ret;
}

AeVector<float, 3> QeTransform::worldScale() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return scale * animation->getBoneScale(targetBoneName);
            }
    }*/

    AeVector<float, 3> _ret = data.scale;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret *= _parent->transform->data.scale;
        _parent = _parent->owner;
    }
    return _ret;
}

AeVector<float, 3> QeTransform::worldFaceEular() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return faceEular +
    animation->getBoneRotateEuler(targetBoneName);
            }
    }*/

     AeVector<float, 3> _ret = data.faceEular;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->data.faceEular;
        _parent = _parent->owner;
    }
    return _ret;
}

AeVector<float, 3> QeTransform::worldFaceVector() { return MATH->eulerAnglesToVector(worldFaceEular()); }

AeVector<float, 3> QeTransform::localFaceVector() { return MATH->eulerAnglesToVector(data.faceEular); }

void QeTransform::setWorldPosition(AeVector<float, 3> &_worldPosition) {
    data.position = _worldPosition;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) data.position -= _parent->transform->data.position;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldScale(AeVector<float, 3> &_worldScale) {
    data.scale = _worldScale;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) data.scale /= _parent->transform->data.scale;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldFaceByEular(AeVector<float, 3> &_worldFace) {
    data.faceEular = _worldFace;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) data.faceEular -= _parent->transform->data.faceEular;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldFaceByVector(AeVector<float, 3> &_worldFaceVector) {
    setWorldFaceByEular(MATH->vectorToEulerAngles(_worldFaceVector));
}

void QeTransform::move(AeVector<float, 3> &_addMove, AeVector<float, 3> &_face, AeVector<float, 3> &_up) {
    data.position = MATH->move(data.position, _addMove, _face, _up);
}

void QeTransform::revolute(AeVector<float, 3> &_addRevolute, AeVector<float, 3> &_centerPosition, bool bFixX, bool bFixY,
                           bool bFixZ) {
    setWorldPosition(MATH->revolute_axis(worldPosition(), _addRevolute, _centerPosition, bFixX, bFixY, bFixZ));
}

QeMatrix4x4f QeTransform::worldTransformMatrix(bool bRotate, bool bFixSize) {
    if (data.targetAnimationOID) {
        QeAnimation *animation = (QeAnimation *)OBJMGR->findComponent(eGAMEOBJECT_Component_Animation, data.targetAnimationOID);
        if (animation) {
            return animation->getBoneTransfrom(data.targetBoneName.c_str()) *
                   MATH->getTransformMatrix(data.position, data.faceEular, data.scale,
                                            GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
        }
    }

    return MATH->getTransformMatrix(worldPosition(), worldFaceEular(), worldScale(),
                                    GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
}
