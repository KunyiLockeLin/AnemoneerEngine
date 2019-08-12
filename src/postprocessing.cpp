#include "header.h"

void QePostProcessing::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    param1.x = initProperty->getXMLValuef("param0");
    param1.y = initProperty->getXMLValuef("param1");
    param1.z = initProperty->getXMLValuef("param2");
    param1.w = initProperty->getXMLValuef("param3");
}
