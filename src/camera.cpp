#include "header.h"

void QeCamera::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE

    // type = eCameraThirdPerson;
    bufferData.vertical_lensRadius.w = component_data.aperture / 2;

    bUpdatePostProcessingOID = false;
    if (component_data.postProcessingOID) bUpdatePostProcessingOID = true;

    GRAP->createRender(component_data.renderType, data.oid, component_data.renderSize);
}

void QeCamera::rotateTarget(AeArray<float, 3> _addRotate) {
    if (component_data.renderType == eRENDER_UI) return;

    AeArray<float, 3> lookAtV = lookAt();
    AeArray<float, 3> pos = owner->transform->worldPosition();
    AeArray<float, 3> vec = {pos - lookAtV};

    AeArray<float, 3> euler = MATH.vectorToEulerAngles(vec);
    if ((euler.y < -85 && _addRotate.y < 0) || (euler.y > 85 && _addRotate.y > 0)) return;
    _addRotate *= component_data.speed;

    if (_addRotate.x) {
        float f = euler.y + _addRotate.y;
        if (f < -85)
            _addRotate.y = -89 - euler.y;
        else if (f > 85)
            _addRotate.y = 89 - euler.y;
    }
    owner->transform->revolute(_addRotate, lookAtV, false, false, true);
}

void QeCamera::rotateTargetByMouse(AeArray<int, 2> mousePos) {
    rotateTarget({0.f, float(mousePos.y - lastMousePos.y), float(mousePos.x - lastMousePos.x)});
    lastMousePos = mousePos;
}

void QeCamera::setMousePos(AeArray<int, 2> mousePos) { lastMousePos = mousePos; }

void QeCamera::zoomInOut(AeArray<int, 2> mousePos) {
    move({0, 0, float(-(mousePos.x - lastMousePos.x) / 10)}, false);
    lastMousePos = mousePos;
}

void QeCamera::move(AeArray<float, 3> _dir, bool bMoveCenter) {
    AeArray<float, 3> lookAtV = lookAt();
    AeArray<float, 3> pos = owner->transform->worldPosition();
    AeArray<float, 3> face = pos - lookAtV;

    if (component_data.renderType == eRENDER_UI) {
        _dir *= component_data.speed;
        pos += _dir;
        owner->transform->setWorldPosition(pos);
        return;
    }

    // forward
    if (_dir.z > 0 && !bMoveCenter && MATH.length(face) < 1) {
        return;
    }
    _dir *= component_data.speed;

    owner->transform->move(_dir, face, component_data.up);
    pos = owner->transform->worldPosition();

    if (bMoveCenter) {
        QeTransform *lookAtTransform =
            (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.lookAtTransformOID);
        lookAtTransform->move(_dir, face, component_data.up);
        owner->transform->setWorldPosition(pos);
    }
}

bool QeCamera::isRaytracing() { return bufferData.pos_rayTracingDepth.w > 0 ? true : false; }

AeArray<float, 3> QeCamera::face() {
    if (component_data.renderType == eRENDER_UI) return {0, 0, 1};
    return MATH.normalize(lookAt() - owner->transform->worldPosition());
}

AeArray<float, 3> QeCamera::lookAt() {
    if (component_data.renderType == eRENDER_UI) {
        AeArray<float, 3> pos = owner->transform->worldPosition();
        return {pos.x, pos.y, pos.z + 1};
    }
    if (component_data.lookAtTransformOID > 0) {
        QeTransform *lookAtTransform =
            (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.lookAtTransformOID);
        return lookAtTransform->worldPosition();
    }
    return {0.f, 0.f, 0.f};
}

void QeCamera::reset() {
    if (component_data.lookAtTransformOID > 0) {
        QeTransform *lookAtTransform =
            (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.lookAtTransformOID);
        lookAtTransform->initialize(lookAtTransform->owner->transform->component_data.property_, lookAtTransform->owner);
    }
    owner->transform->initialize(owner->transform->component_data.property_, owner);
    // initialize(initProperty, owner);
}

void QeCamera::setLookAtTransformOID(int _lookAtransformOID) {
    if (!_lookAtransformOID) return;

    QeTransform *lookAtTransform1 = (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, _lookAtransformOID);
    if (!lookAtTransform1) return;
    QeTransform *lookAtTransform2 =
        (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.lookAtTransformOID);
    if (!lookAtTransform2) return;

    lookAtTransform2->setWorldPosition(lookAtTransform1->worldPosition());
    owner->transform->component_data.position = {20.f, 0.f, 1.f};
}

void QeCamera::updatePreRender() {
    if (bUpdatePostProcessingOID && component_data.postProcessingOID) {
        if (GRAP->addPostProcssing(component_data.renderType, data.oid, component_data.postProcessingOID)) {
            bUpdatePostProcessingOID = false;
        }
    }

    AeArray<float, 3> pos = owner->transform->worldPosition();
    bufferData.pos_rayTracingDepth = pos;
    AeArray<float, 3> lookAtV = lookAt();
    AeArray<float, 3> face = lookAtV - pos;
    float focusDist = MATH.length(face);
    face = MATH.normalize(face);
    // focusDist = 1;
    if (component_data.renderType == eRENDER_UI) {
        bufferData.horizontal_aspect.w = ((float)component_data.renderSize.width) / component_data.renderSize.height;
    } else {
        bufferData.view = MATH.lookAt(pos, lookAtV, component_data.up);
        bufferData.projection =
            MATH.perspective(component_data.fov, bufferData.horizontal_aspect.w, component_data.fnear, component_data.ffar);

        float half_height = -1.0f / bufferData.projection._11;            // tan(theta/2);
        float half_width = bufferData.horizontal_aspect.w * half_height;  // 1.0/cam.projection[0][0];

        AeArray<float, 3> w = face;  // vec3(cam.view[0][2], cam.view[1][2], cam.view[2][2]);
        AeArray<float, 3> u = {bufferData.view._00, bufferData.view._10,
                                  bufferData.view._20};  // cross(vup, w);
        AeArray<float, 3> v = {bufferData.view._01, bufferData.view._11,
                                  bufferData.view._21};  // cross(w, u);
        bufferData.lowerLeftCorner = pos - u * half_width * focusDist - v * half_height * focusDist + w * focusDist;
        bufferData.horizontal_aspect = u * 2 * half_width * focusDist;
        bufferData.vertical_lensRadius = v * 2 * half_height * focusDist;
    }
}
