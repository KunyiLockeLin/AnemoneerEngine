#pragma once
#include "header.h"

class QeMaterial : public QeComponent {
   public:
    QeMaterial(QeSceneManagerKey &_key) : QeComponent(_key) {}
    ~QeMaterial() {}

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);

    bool bAlpha;
    QeAssetMaterial materialData;
};
