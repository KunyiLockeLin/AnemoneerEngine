#pragma once

#include "header.h"

class QeComponent{
   public:
    QeComponent(AeObjectManagerKey &_key) {}
    ~QeComponent() {}
    QeObject *owner = nullptr;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner) {}
    virtual void clear() {}

    virtual void updatePreRender() {}
    virtual void updatePostRedner() {}
};

class QeObject : public QeComponent {
   public:
    QeObject(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeObject() {}

    QeTransform *transform;

    std::vector<QeComponent *> components;
    std::vector<QeComponent *> children;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();
    virtual void updatePreRender();
    virtual void updatePostRedner();
};
