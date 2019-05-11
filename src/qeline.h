#pragma once
#include "qeheader.h"

class QeLine : public QeModel {
   public:
    QeLine(QeObjectMangerKey &_key) : QeModel(_key) {}

    int targetTransformOID;
    QeVector3f color;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    // virtual void clear() {}
    virtual void update1();
    // virtual void update2() {}

    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
