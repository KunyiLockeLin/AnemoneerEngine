#pragma once
#include "header.h"

class QeMaterial : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeMaterial, AeGameObjectComponentMaterialData)

    virtual void initialize(AeXMLNode *_property, QeObject *_owner);

    QeAssetMaterial materialData;
};
