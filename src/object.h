#pragma once

#include "header.h"

#define COMPONENT_CLASS_DECLARE(component_type) \
    Qe##component_type(AeObjectManagerKey &_key) : QeComponent(_key) {} \
    ~Qe##component_type() {} \
    AeGameObjectDataComponent##component_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner); \

#define COMPONENT_CLASS_DECLARE_PARENT(component_type, parent_component_type) \
    Qe##component_type(AeObjectManagerKey &_key) : Qe##parent_component_type(_key) {} \
    ~Qe##component_type() {} \
    AeGameObjectDataComponent##component_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_INITIALIZE(component_type, ...) \
     Qe##component_type(AeObjectManagerKey &_key) : QeComponent(_key), __VA_ARGS__ {}              \
    ~Qe##component_type() {}                                                          \
    AeGameObjectDataComponent##component_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_PARENT_INITIALIZE(component_type, parent_component_type, ...) \
    Qe##component_type(AeObjectManagerKey &_key) : Qe##parent_component_type(_key), __VA_ARGS__ {}    \
    ~Qe##component_type() {}                                                            \
    AeGameObjectDataComponent##component_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_INITIALIZE       \
    QeComponent::initialize(_property, _owner); \
    component_data.read(*_property); \

#define COMPONENT_INITIALIZE_PARENT(parent_component_type) \
    Qe##parent_component_type::initialize(_property, _owner); \
    component_data.read(*_property); \

class QeComponent {
   public:
    QeComponent(AeObjectManagerKey &_key) {}
    ~QeComponent() {}
    QeObject *owner = nullptr;
    AeGameObjectData data;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
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
