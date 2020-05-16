#pragma once
#include "header.h"

class QeGrid : public QeLine {
   public:
    COMPONENT_CLASS_DECLARE_PARENT(QeGrid, QeLine, AeGameObjectComponentGridData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void updatePreRender();
};
