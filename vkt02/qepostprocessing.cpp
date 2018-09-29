#include "qeheader.h"


void QePostProcessing::initialize(QeAssetXML* _property, QeObject* _owner) {
	QeComponent::initialize(_property, _owner);

	AST->getXMLfValue(&param[0], initProperty, 1, "param0");
	AST->getXMLfValue(&param[1], initProperty, 1, "param1");
	AST->getXMLfValue(&param[2], initProperty, 1, "param2");
	AST->getXMLfValue(&param[3], initProperty, 1, "param3");
}