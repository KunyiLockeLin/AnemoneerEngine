#pragma once
#include "header.h"

class QeAxis : public QeLine {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QeAxis, QeLine, AeGameObjectComponentAxisData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void updatePreRender();
};
