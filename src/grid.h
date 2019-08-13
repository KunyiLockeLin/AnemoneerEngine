#pragma once
#include "header.h"

class QeGrid : public QeLine {
   public:
    QeGrid(AeObjectManagerKey &_key) : QeLine(_key) {}

    int column, row;
    QeVector3f color;

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
    virtual void update1();
};
