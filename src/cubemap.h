#pragma once
#include "header.h"

class QeCubemap : public QeModel {
   public:
    QeCubemap(AeObjectManagerKey &_key) : QeModel(_key) {}
    //~QeCubemap() {}

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
