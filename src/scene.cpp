#include "header.h"

void QeScene::initialize(QeAssetXML * _property, QeObject * _owner) {
    QeComponent::initialize(_property, _owner);
    clear();
    GRAP->initialize();
    QeAssetXML *node = CONFIG->getXMLNode("setting.environment");
    node->setXMLValue("currentSceneEID", std::to_string(eid).c_str());

    for (int index = 0; index < initProperty->nexts.size(); ++index) {
        children.push_back(OBJMGR->spwanComponent(initProperty->nexts[index], nullptr));
    }
    LOG("current Scene: " + name + " " + eid);
}
