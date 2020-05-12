#pragma once
#include "header.h"

class QeLine : public QeModel {
   public:
    QeLine(AeObjectManagerKey &_key) : QeModel(_key) {}

    int targetTransformOID;
    QeVector3f color;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    // virtual void clear() {}
    virtual void update1();
    // virtual void update2() {}

    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
