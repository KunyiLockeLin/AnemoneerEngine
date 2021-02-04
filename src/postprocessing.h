#pragma once
#include "header.h"

class QePostProcessing : public QeComponent {
   public:
    QePostProcessing(AeObjectManagerKey &_key) : QeComponent(_key) {}
    //~QePostProcessing();

    AeGameObjectComponentPostProcessingData component_data;

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
};
