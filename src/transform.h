#pragma once
#include "header.h"

class QeTransform : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeTransform, AeGameObjectComponentTransformData)

    virtual void updatePreRender();

    // Face is Euler angles, (roll, pitch, yaw) or (bank, attitude, heading).
    AeArray<float, 3> worldPosition();
    AeArray<float, 3> worldScale();
    AeArray<float, 3> worldFaceEular();
    AeArray<float, 3> worldFaceVector();
    AeArray<float, 3> localFaceVector();

    void setWorldPosition(AeArray<float, 3> &_worldPosition);
    void setWorldScale(AeArray<float, 3> &_worldScale);
    void setWorldFaceByEular(AeArray<float, 3> &_worldFaceEular);
    void setWorldFaceByVector(AeArray<float, 3> &_worldFaceVector);

    void move(AeArray<float, 3> &_addMove, AeArray<float, 3> &_face, AeArray<float, 3> &_up);
    void revolute(AeArray<float, 3> &_addRevolute, AeArray<float, 3> &_centerPosition, bool bFixX = false, bool bFixY = false,
                  bool bFixZ = false);

    QeMatrix4x4f worldTransformMatrix(bool bRotate = true, bool bFixSize = false);
};
