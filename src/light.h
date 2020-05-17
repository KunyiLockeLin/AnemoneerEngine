#pragma once
#include "header.h"

struct QeDataLight {
    AeVector<float, 4> pos;
    AeVector<float, 4> dir;
    AeVector<float, 4> color;
    AeVector<float, 4> param;  // 1: type, 2: intensity, 3: coneAngle
};

class QeLight : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeLight, AeGameObjectComponentLightData)

    QeDataLight bufferData;
    bool bUpdate;

    virtual void clear();
    virtual void updatePreRender();
};
