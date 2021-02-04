#include "header.h"

void QeScene::initialize(AeXMLNode * _property, QeObject * _owner) {
    QeComponent::initialize(_property, _owner);
    clear();
    GRAP->initialize();
    AeXMLNode *node = CONFIG->getXMLNode("setting.environment");
    node->setXMLValue("currentSceneEID", std::to_string(data.eid).c_str());

    for (int index = 0; index < data.property_->data->nexts.size(); ++index) {
        children.push_back(OBJMGR->spwanComponent(data.property_->data->nexts[index], nullptr));
    }
    LOG("current Scene: " + name + " " + eid);
}
