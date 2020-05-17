#pragma once
#include "header.h"

class QeTransform : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeTransform, AeGameObjectComponentTransformData)

    virtual void updatePreRender();

    // Face is Euler angles, (roll, pitch, yaw) or (bank, attitude, heading).
    AeVector<float, 3> worldPosition();
    AeVector<float, 3> worldScale();
    AeVector<float, 3> worldFaceEular();
    AeVector<float, 3> worldFaceVector();
    AeVector<float, 3> localFaceVector();

    void setWorldPosition(AeVector<float, 3> &_worldPosition);
    void setWorldScale(AeVector<float, 3> &_worldScale);
    void setWorldFaceByEular(AeVector<float, 3> &_worldFaceEular);
    void setWorldFaceByVector(AeVector<float, 3> &_worldFaceVector);

    void move(AeVector<float, 3> &_addMove, AeVector<float, 3> &_face, AeVector<float, 3> &_up);
    void revolute(AeVector<float, 3> &_addRevolute, AeVector<float, 3> &_centerPosition, bool bFixX = false, bool bFixY = false,
                  bool bFixZ = false);

    QeMatrix4x4f worldTransformMatrix(bool bRotate = true, bool bFixSize = false);
};
