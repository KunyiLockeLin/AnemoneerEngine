#include "header.h"

void QeMaterial::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE

    materialData.value.baseColor = component_data.baseColor;
    materialData.value.baseColor.w = component_data.baseValueRate;
    materialData.value.metallicRoughnessEmissive.x = component_data.metallic;
    materialData.value.metallicRoughnessEmissive.y = component_data.roughness;
    materialData.value.metallicRoughnessEmissive.z = component_data.emissive;

    materialData.image.pBaseColorMap = nullptr;
    materialData.image.pCubeMap = nullptr;
    materialData.image.pNormalMap = nullptr;
    materialData.image.pMetallicRoughnessMap = nullptr;

    if (!component_data.baseMap.empty()) materialData.image.pBaseColorMap = G_AST.getImage(component_data.baseMap.c_str(), false, true);
    if (!component_data.cubeMap.empty()) materialData.image.pCubeMap = G_AST.getImage(component_data.cubeMap.c_str(), true, true);
    if (!component_data.normalMap.empty()) materialData.image.pNormalMap = G_AST.getImage(component_data.normalMap.c_str(), false);
    if (!component_data.metallicRoughnessMap.empty())
        materialData.image.pMetallicRoughnessMap = G_AST.getImage(component_data.metallicRoughnessMap.c_str(), false);
}
