#pragma once

#include "header.h"

#define COMPONENT_CLASS_DECLARE(class_name, componet_data_type) \
    class_name(AeObjectManagerKey &_key) : QeComponent(_key) {} \
    ~class_name() {} \
    componet_data_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner); \

#define COMPONENT_CLASS_DECLARE_PARENT(class_name, parent_class, componet_data_type) \
    class_name(AeObjectManagerKey &_key) : parent_class(_key) {} \
    ~class_name() {} \
    componet_data_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_INITIALIZE(class_name, componet_data_type, ...) \
    class_name(AeObjectManagerKey &_key) : QeComponent(_key), __VA_ARGS__ {}              \
    ~class_name() {}                                                          \
    componet_data_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_CLASS_DECLARE_PARENT_INITIALIZE(class_name, parent_class, componet_data_type, ...) \
    class_name(AeObjectManagerKey &_key) : parent_class(_key), __VA_ARGS__ {}    \
    ~class_name() {}                                                            \
    componet_data_type component_data; \
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

#define COMPONENT_INITIALIZE       \
    QeComponent::initialize(_property, _owner); \
    component_data.read(*_property); \

#define COMPONENT_INITIALIZE_PARENT(parent_class) \
    parent_class::initialize(_property, _owner); \
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
