#pragma once
#include "header.h"

class QeCubemap : public QeModel {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QeCubemap, QeModel, AeGameObjectComponentCubemapData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
