#include "header.h"

void QeMaterial::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    bAlpha = initProperty->getXMLValueb("alpha");
    materialData.value.baseColor = initProperty->getXMLValueRGB("baseColor");
    materialData.value.baseColor.w = initProperty->getXMLValuef("baseValueRate");
    materialData.value.metallicRoughnessEmissive.x = initProperty->getXMLValuef("metallic");
    materialData.value.metallicRoughnessEmissive.y = initProperty->getXMLValuef("roughness");
    materialData.value.metallicRoughnessEmissive.z = initProperty->getXMLValuef("emissive");

    materialData.image.pBaseColorMap = nullptr;
    materialData.image.pCubeMap = nullptr;
    materialData.image.pNormalMap = nullptr;
    materialData.image.pMetallicRoughnessMap = nullptr;

    const char *c = initProperty->getXMLValue("baseMap");
    if (c && strlen(c)) materialData.image.pBaseColorMap = AST->getImage(c, false, true);
    c = initProperty->getXMLValue("cubeMap");
    if (c && strlen(c)) materialData.image.pCubeMap = AST->getImage(c, true, true);
    c = initProperty->getXMLValue("normalMap");
    if (c && strlen(c)) materialData.image.pNormalMap = AST->getImage(c, false);
    c = initProperty->getXMLValue("metallicRoughnessMap");
    if (c && strlen(c)) materialData.image.pMetallicRoughnessMap = AST->getImage(c, false);
}
