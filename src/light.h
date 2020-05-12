#pragma once
#include "header.h"

struct QeDataLight {
    QeVector4f pos;
    QeVector4f dir;
    QeVector4f color;
    QeVector4f param;  // 1: type, 2: intensity, 3: coneAngle
};

class QeLight : public QeComponent {
   public:
    QeLight(AeObjectManagerKey &_key) : QeComponent(_key) {}
    //~QeLight() {}

    QeDataLight bufferData;
    bool bUpdate;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();
    virtual void update1();
    virtual void update2() {}
};
