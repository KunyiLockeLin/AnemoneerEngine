#include "qeheader.h"

void QePostProcessing::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    AST->getXMLfValue(&param1.x, initProperty, 1, "param0");
    AST->getXMLfValue(&param1.y, initProperty, 1, "param1");
    AST->getXMLfValue(&param1.z, initProperty, 1, "param2");
    AST->getXMLfValue(&param1.w, initProperty, 1, "param3");
}
