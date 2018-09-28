#include "qeheader.h"

void QeMaterial::initialize(QeAssetXML* _property, QeObject* _owner) {

	QeComponent::initialize(_property, _owner);

	AST->getXMLbValue(&bAlpha, initProperty, 1, "alpha");
	AST->getXMLfValue(&materialData.baseColor.x, initProperty, 1, "baseColorR");
	AST->getXMLfValue(&materialData.baseColor.y, initProperty, 1, "baseColorG");
	AST->getXMLfValue(&materialData.baseColor.z, initProperty, 1, "baseColorB");
	AST->getXMLfValue(&materialData.baseColor.w, initProperty, 1, "baseValueRate");
	AST->getXMLfValue(&materialData.metallicRoughnessEmissive.x, initProperty, 1, "metallic");
	AST->getXMLfValue(&materialData.metallicRoughnessEmissive.y, initProperty, 1, "roughness");
	AST->getXMLfValue(&materialData.metallicRoughnessEmissive.z, initProperty, 1, "emissive");

	const char * c = AST->getXMLValue(initProperty, 1, "baseMap");
	if (c && strlen(c) ) imageData.pBaseColorMap = AST->getImage(c, false);
	c = AST->getXMLValue(initProperty, 1, "normalMap");
	if (c && strlen(c)) imageData.pNormalMap = AST->getImage(c, false);
	c = AST->getXMLValue(initProperty, 1, "environmentMap");
	if (c && strlen(c)) imageData.pEnvironmentMap = AST->getImage(c, false);

	c = AST->getXMLValue(initProperty, 1, "vert");
	if (c && strlen(c)) shaderData.vert = AST->getShader(c);
	c = AST->getXMLValue(initProperty, 1, "tesc");
	if (c && strlen(c)) shaderData.tesc = AST->getShader(c);
	c = AST->getXMLValue(initProperty, 1, "tese");
	if (c && strlen(c)) shaderData.tese = AST->getShader(c);
	c = AST->getXMLValue(initProperty, 1, "geom");
	if (c && strlen(c)) shaderData.geom = AST->getShader(c);
	c = AST->getXMLValue(initProperty, 1, "frag");
	if (c && strlen(c)) shaderData.frag = AST->getShader(c);
}