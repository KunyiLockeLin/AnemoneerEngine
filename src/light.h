#pragma once
#include "header.h"

struct QeDataLight {
    AeArray<float, 4> pos;
    AeArray<float, 4> dir;
    AeArray<float, 4> color;
    AeArray<float, 4> param;  // 1: type, 2: intensity, 3: coneAngle
};

class QeLight : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(Light)

    QeDataLight bufferData;
    bool bUpdate;

    virtual void clear();
    virtual void updatePreRender();
};
