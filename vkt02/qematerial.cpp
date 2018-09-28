#include "qeheader.h"

void QeMaterial::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	AST->getXMLbValue(&bAlpha, initProperty, 1, "alpha");
	AST->getXMLfValue(&materialData.value.baseColor.x, initProperty, 1, "baseColorR");
	AST->getXMLfValue(&materialData.value.baseColor.y, initProperty, 1, "baseColorG");
	AST->getXMLfValue(&materialData.value.baseColor.z, initProperty, 1, "baseColorB");
	AST->getXMLfValue(&materialData.value.baseColor.w, initProperty, 1, "baseValueRate");
	AST->getXMLfValue(&materialData.value.metallicRoughnessEmissive.x, initProperty, 1, "metallic");
	AST->getXMLfValue(&materialData.value.metallicRoughnessEmissive.y, initProperty, 1, "roughness");
	AST->getXMLfValue(&materialData.value.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	const char * c = AST->getXMLValue(initProperty, 1, "baseMap");
	if (c && strlen(c) ) materialData.image.pBaseColorMap = AST->getImage(c, false);;
	c = AST->getXMLValue(initProperty, 1, "cubeMap");
	if (c && strlen(c)) materialData.image.pCubeMap = AST->getImage(c, false);
	c = AST->getXMLValue(initProperty, 1, "normalMap");
	if (c && strlen(c)) materialData.image.pNormalMap = AST->getImage(c, false);
}