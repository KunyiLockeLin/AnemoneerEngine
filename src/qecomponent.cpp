#include "qeheader.h"

void QeComponent::initialize(QeAssetXML *_property, QeObject *_owner) {
    owner = _owner;
    initProperty = _property;
    name = initProperty->key;

    AST->getXMLiValue(&oid, initProperty, 1, "oid");
    AST->getXMLiValue(&eid, initProperty, 1, "eid");

    int type = 0;
    AST->getXMLiValue(&type, initProperty, 1, "type");
    componentType = (QeComponentType)type;
}
