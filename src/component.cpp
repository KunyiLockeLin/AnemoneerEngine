#include "header.h"

void QeComponent::initialize(QeAssetXML *_property, QeObject *_owner) {
    owner = _owner;
    initProperty = _property;
    name = initProperty->key;

    oid = initProperty->getXMLValuei("oid");
    eid = initProperty->getXMLValuei("eid");

    componentType = (QeComponentType)initProperty->getXMLValuei("type");
}
