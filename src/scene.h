#pragma once

#include "header.h"

class QeScene : public QeObject {
   public:
    QeScene(AeObjectManagerKey &_key) : QeObject(_key) {}
    ~QeScene() {}

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);
};
