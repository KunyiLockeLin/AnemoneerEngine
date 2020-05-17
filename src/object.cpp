#include "header.h"

void QeComponent::initialize(AeXMLNode *_property, QeObject *_owner) {
    owner = _owner;
    data.read(*_property);
}

void QeObject::initialize(AeXMLNode *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    AeXMLNode *node = data.property_->getXMLNode("components");
    if (node != nullptr && node->data->nexts.size() > 0) {
        for (int index = 0; index < node->data->nexts.size(); ++index) {
            components.push_back(OBJMGR->spwanComponent(node->data->nexts[index], this));
        }
    }

    node = data.property_->getXMLNode("children");
    if (node != nullptr && node->data->nexts.size() > 0) {
        for (int index = 0; index < node->data->nexts.size(); ++index) {
            children.push_back(OBJMGR->spwanComponent(node->data->nexts[index], this));
        }
    }
}

void QeObject::clear() {
    std::vector<QeComponent *>::iterator it = components.begin();
    while (it != components.end()) {
        OBJMGR->removeComponent(*it);
        ++it;
    }
    components.clear();

    std::vector<QeComponent *>::iterator it1 = children.begin();
    while (it1 != children.end()) {
        OBJMGR->removeComponent(*it1);
        ++it1;
    }
    children.clear();

    /*if (!parent) {
            eraseElementFromVector(SCENE->objects, this);
            parent = nullptr;
    }*/
}

void QeObject::updatePreRender() {
    std::vector<QeComponent *>::iterator it = components.begin();
    while (it != components.end()) {
        (*it)->updatePreRender();
        ++it;
    }
    std::vector<QeComponent *>::iterator it1 = children.begin();
    while (it1 != children.end()) {
        (*it1)->updatePreRender();
        ++it1;
    }
}

void QeObject::updatePostRedner() {
    std::vector<QeComponent *>::iterator it = components.begin();
    while (it != components.end()) {
        (*it)->updatePostRedner();
        ++it;
    }
    std::vector<QeComponent *>::iterator it1 = children.begin();
    while (it1 != children.end()) {
        (*it1)->updatePostRedner();
        ++it1;
    }
}
