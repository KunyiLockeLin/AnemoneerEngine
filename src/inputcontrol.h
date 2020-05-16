#pragma once
#include "header.h"

class QeInputControl : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeInputControl, AeGameObjectComponentInputControlData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();

    virtual void updateInput();
};
