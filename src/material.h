#pragma once
#include "header.h"

class QeMaterial : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(Material)

    QeAssetMaterial materialData;
};
