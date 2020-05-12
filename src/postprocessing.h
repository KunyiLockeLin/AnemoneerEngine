#pragma once
#include "header.h"

class QePostProcessing : public QeComponent {
   public:
    QePostProcessing(AeObjectManagerKey &_key) : QeComponent(_key) {}
    //~QePostProcessing();

    QeVector4f param1;
    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
};
