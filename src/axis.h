#pragma once
#include "header.h"

class QeAxis : public QeLine {
   public:
    QeAxis(AeObjectManagerKey &_key) : QeLine(_key) {}

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void update1();
};
