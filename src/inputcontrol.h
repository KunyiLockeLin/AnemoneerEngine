#pragma once
#include "component.h"

class QeInputControl : public QeComponent {
   public:
    QeInputControl(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeInputControl() {}

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
    virtual void clear();

    virtual void updateInput();
};
