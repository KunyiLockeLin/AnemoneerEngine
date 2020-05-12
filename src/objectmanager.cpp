#include "header.h"

void AeObjectManager::loadScene(int _eid) {
    AeXMLNode *node = G_AST->getXMLEditNode(eScene, _eid);

    if (SCENE)
        SCENE->initialize(node, nullptr);
    else
        SCENE = (QeScene*)spwanComponent(node, nullptr);
}

AeObjectManager::~AeObjectManager() {
    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it = components.begin();
    std::vector<QeComponent *>::iterator it1;

    while (it != components.end()) {
        it1 = it->second.begin();
        while (it1 != it->second.end()) {
            (*it1)->clear();
            delete *it1;
            ++it1;
        }
        it->second.clear();
        ++it;
    }
    components.clear();

    it = unActiveComponents.begin();
    while (it != unActiveComponents.end()) {
        it1 = it->second.begin();
        while (it1 != it->second.end()) {
            (*it1)->clear();
            delete *it1;
            ++it1;
        }
        it->second.clear();
        ++it;
    }
    unActiveComponents.clear();
}

void AeObjectManager::clear() {
    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it = components.begin();
    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it11;

    std::vector<QeComponent *>::iterator it1;

    while (it != components.end()) {
        if (it->second.size()) {
            it11 = unActiveComponents.find(it->first);

            if (it11 == unActiveComponents.end()) {
                unActiveComponents.insert(std::pair<QeComponentType, std::vector<QeComponent *>>(it->first, {}));
                it11 = unActiveComponents.find(it->first);
            }

            it1 = it->second.begin();

            while (it1 != it->second.end()) {
                (*it1)->clear();
                it11->second.push_back(*it1);
                ++it1;
            }
            it->second.clear();
        }
        ++it;
    }
}

QeComponent *AeObjectManager::spwanComponent(AeXMLNode *_property, QeObject *_owner) {
    QeComponent *_component = nullptr;

    QeComponentType _type = (QeComponentType)_property->getXMLValuei("type");

    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it = unActiveComponents.find(_type);
    if (it != unActiveComponents.end() && it->second.size() > 0) {
        size_t size = it->second.size();
        _component = it->second[size - 1];
        it->second.pop_back();
    } else {
        switch (_type) {
            case eComponent_transform:
                _component = new QeTransform(key);
                break;
            case eComponent_camera:
                _component = new QeCamera(key);
                break;
            case eComponent_postprocessing:
                _component = new QePostProcessing(key);
                break;
            case eComponent_light:
                _component = new QeLight(key);
                break;
            case eComponent_line:
                _component = new QeLine(key);
                break;
            case eComponent_axis:
                _component = new QeAxis(key);
                break;
            case eComponent_grid:
                _component = new QeGrid(key);
                break;
            case eComponent_model:
                _component = new QeModel(key);
                break;
            case eComponent_animation:
                _component = new QeAnimation(key);
                break;
            case eComponent_plane:
                _component = new QePlane(key);
                break;
            case eComponent_cubemap:
                _component = new QeCubemap(key);
                break;
            case eComponent_partical:
                _component = new QeParticle(key);
                break;
            case eComponent_material:
                _component = new QeMaterial(key);
                break;
            case eComponent_inputControl:
                _component = new QeInputControl(key);
                break;
            case eComponent_rendersetting:
                _component = new QeRenderSetting(key);
                break;
            case eObject:
                _component = new QeObject(key);
                break;
            case eScene:
                _component = new QeScene(key);
                break;
        }
    }
    it = components.find(_type);
    if (it != components.end()) {
        it->second.push_back(_component);
    } else {
        components.insert(std::pair<QeComponentType, std::vector<QeComponent *>>(_type, {_component}));
    }
    _component->initialize(_property, _owner);

    return _component;
}

QeComponent *AeObjectManager::findComponent(QeComponentType type, int _oid) {
    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it = components.find(type);

    if (it != components.end()) {
        std::vector<QeComponent *>::iterator it1 = it->second.begin();
        while (it1 != it->second.end()) {
            if ((*it1)->oid == _oid) {
                return (*it1);
            }
            ++it1;
        }
        ++it;
    }
    return nullptr;
}

bool AeObjectManager::removeComponent(QeComponent *_component) {
    std::map<QeComponentType, std::vector<QeComponent *>>::iterator it = components.find(_component->componentType);
    if (it != components.end()) {
        int index = findElementFromVector(it->second, _component);

        if (index == INDEX_NONE) return false;
        _component->clear();
        it->second.erase(it->second.begin() + index);

        it = unActiveComponents.find(_component->componentType);
        if (it != unActiveComponents.end()) {
            it->second.push_back(_component);
        } else {
            unActiveComponents.insert(
                std::pair<QeComponentType, std::vector<QeComponent *>>(_component->componentType, {_component}));
        }
        return true;
    }
    return false;
}
