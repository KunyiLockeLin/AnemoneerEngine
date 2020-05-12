#pragma once
#include "header.h"

class QeMaterial : public QeComponent {
   public:
    QeMaterial(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeMaterial() {}

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

    bool bAlpha;
    QeAssetMaterial materialData;
};
