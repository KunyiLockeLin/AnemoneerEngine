#pragma once
#include "header.h"

class QeCubemap : public QeModel {
   public:
    QeCubemap(QeSceneManagerKey &_key) : QeModel(_key) {}
    //~QeCubemap() {}

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
