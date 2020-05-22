#include "header.h"

void QeTransform::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE
    _owner->transform = this;
}

void QeTransform::updatePreRender() {
    if (component_data.rotateSpeed.x != 0.f || component_data.rotateSpeed.y != 0.f || component_data.rotateSpeed.z != 0.f) {
        component_data.faceEular += (component_data.rotateSpeed * ENGINE->deltaTime * 30.f);
    }

    if ((owner && owner->owner && owner->owner->transform) &&
        (component_data.revoluteSpeed.x != 0.f || component_data.revoluteSpeed.y != 0.f || component_data.revoluteSpeed.z != 0.f)) {
        revolute((component_data.revoluteSpeed * ENGINE->deltaTime * 30.f), owner->owner->transform->worldPosition(),
                 component_data.revoluteFixAxis.x, component_data.revoluteFixAxis.y, component_data.revoluteFixAxis.z);
    }
}

AeArray<float, 3> QeTransform::worldPosition() {
    if (component_data.targetAnimationOID) {
        QeAnimation *animation =
            (QeAnimation *)OBJMGR->findComponent(eGAMEOBJECT_Component_Animation, component_data.targetAnimationOID);
        if (animation) {
            AeArray<float, 4> vec = {component_data.position, 1.f};
            return animation->getBoneTransfrom(component_data.targetBoneName.c_str()) * vec;
        }
    }

    AeArray<float, 3> _ret = component_data.position;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->component_data.position;
        _parent = _parent->owner;
    }
    return _ret;
}

AeArray<float, 3> QeTransform::worldScale() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return scale * animation->getBoneScale(targetBoneName);
            }
    }*/

    AeArray<float, 3> _ret = component_data.scale;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret *= _parent->transform->component_data.scale;
        _parent = _parent->owner;
    }
    return _ret;
}

AeArray<float, 3> QeTransform::worldFaceEular() {
    /*if (targetAnimationOID) {
            QeAnimation* animation =
    (QeAnimation*)OBJMGR->findComponent(eComponent_animation, targetAnimationOID);
            if (animation) {
                    return faceEular +
    animation->getBoneRotateEuler(targetBoneName);
            }
    }*/

     AeArray<float, 3> _ret = component_data.faceEular;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) _ret += _parent->transform->component_data.faceEular;
        _parent = _parent->owner;
    }
    return _ret;
}

AeArray<float, 3> QeTransform::worldFaceVector() { return MATH.eulerAnglesToVector(worldFaceEular()); }

AeArray<float, 3> QeTransform::localFaceVector() { return MATH.eulerAnglesToVector(component_data.faceEular); }

void QeTransform::setWorldPosition(AeArray<float, 3> &_worldPosition) {
    component_data.position = _worldPosition;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) component_data.position -= _parent->transform->component_data.position;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldScale(AeArray<float, 3> &_worldScale) {
    component_data.scale = _worldScale;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) component_data.scale /= _parent->transform->component_data.scale;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldFaceByEular(AeArray<float, 3> &_worldFace) {
    component_data.faceEular = _worldFace;
    QeObject *_parent = owner->owner;

    while (_parent) {
        if (_parent->transform) component_data.faceEular -= _parent->transform->component_data.faceEular;
        _parent = _parent->owner;
    }
}

void QeTransform::setWorldFaceByVector(AeArray<float, 3> &_worldFaceVector) {
    setWorldFaceByEular(MATH.vectorToEulerAngles(_worldFaceVector));
}

void QeTransform::move(AeArray<float, 3> &_addMove, AeArray<float, 3> &_face, AeArray<float, 3> &_up) {
    component_data.position = MATH.move(component_data.position, _addMove, _face, _up);
}

void QeTransform::revolute(AeArray<float, 3> &_addRevolute, AeArray<float, 3> &_centerPosition, bool bFixX, bool bFixY,
                           bool bFixZ) {
    setWorldPosition(MATH.revolute_axis(worldPosition(), _addRevolute, _centerPosition, bFixX, bFixY, bFixZ));
}

QeMatrix4x4f QeTransform::worldTransformMatrix(bool bRotate, bool bFixSize) {
    if (component_data.targetAnimationOID) {
        QeAnimation *animation =
            (QeAnimation *)OBJMGR->findComponent(eGAMEOBJECT_Component_Animation, component_data.targetAnimationOID);
        if (animation) {
            return animation->getBoneTransfrom(component_data.targetBoneName.c_str()) *
                   MATH.getTransformMatrix(component_data.position, component_data.faceEular, component_data.scale,
                                            GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
        }
    }

    return MATH.getTransformMatrix(worldPosition(), worldFaceEular(), worldScale(),
                                    GRAP->getTargetCamera()->owner->transform->worldPosition(), bRotate, bFixSize);
}
