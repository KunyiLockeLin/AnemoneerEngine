#pragma once

#include "header.h"

class QeObject : public QeComponent {
   public:
    QeObject(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeObject() {}

    QeTransform *transform;

    std::vector<QeComponent *> components;
    std::vector<QeComponent *> children;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void clear();
    virtual void update1();
    virtual void update2();
};
