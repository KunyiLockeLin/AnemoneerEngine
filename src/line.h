#pragma once
#include "header.h"

class QeLine : public QeModel {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(Line, Model)

    virtual void updatePreRender();

    // virtual QeDataDescriptorSetModel createDescriptorSetModel();
};
