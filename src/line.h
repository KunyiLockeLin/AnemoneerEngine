#pragma once
#include "header.h"

class QeLine : public QeModel {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QeLine, QeModel, AeGameObjectComponentLineData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void updatePreRender();

    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
