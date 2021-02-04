#include "header.h"

void AeObjectManager::loadScene(int _eid) {
    AeXMLNode *node = G_AST->getXMLEditNode(eGAMEOBJECT_Scene, _eid);

    if (SCENE)
        SCENE->initialize(node, nullptr);
    else
        SCENE = (QeScene*)spwanComponent(node, nullptr);
}

AeObjectManager::~AeObjectManager() {
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it = active_components.begin();
    std::vector<QeComponent *>::iterator it1;

    while (it != active_components.end()) {
        it1 = it->second.begin();
        while (it1 != it->second.end()) {
            (*it1)->clear();
            delete *it1;
            ++it1;
        }
        it->second.clear();
        ++it;
    }
    active_components.clear();

    it = idle_components.begin();
    while (it != idle_components.end()) {
        it1 = it->second.begin();
        while (it1 != it->second.end()) {
            (*it1)->clear();
            delete *it1;
            ++it1;
        }
        it->second.clear();
        ++it;
    }
    idle_components.clear();
}

void AeObjectManager::clear() {
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it = active_components.begin();
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it11;

    std::vector<QeComponent *>::iterator it1;

    while (it != active_components.end()) {
        if (it->second.size()) {
            it11 = idle_components.find(it->first);

            if (it11 == idle_components.end()) {
                idle_components.insert(std::pair<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>(it->first, {}));
                it11 = idle_components.find(it->first);
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

    AE_GAMEOBJECT_TYPE _type = (AE_GAMEOBJECT_TYPE)_property->getXMLValue<int>("type");

    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it = idle_components.find(_type);
    if (it != idle_components.end() && it->second.size() > 0) {
        size_t size = it->second.size();
        _component = it->second[size - 1];
        it->second.pop_back();
    } else {
        switch (_type) {
            case eGAMEOBJECT_Component_Transform:
                _component = new QeTransform(key);
                break;
            case eGAMEOBJECT_Component_Camera:
                _component = new QeCamera(key);
                break;
            case eGAMEOBJECT_Component_PostProcessing:
                _component = new QePostProcessing(key);
                break;
            case eGAMEOBJECT_Component_Light:
                _component = new QeLight(key);
                break;
            case eGAMEOBJECT_Component_Line:
                _component = new QeLine(key);
                break;
            case eGAMEOBJECT_Component_Axis:
                _component = new QeAxis(key);
                break;
            case eGAMEOBJECT_Component_Grid:
                _component = new QeGrid(key);
                break;
            case eGAMEOBJECT_Component_Model:
                _component = new QeModel(key);
                break;
            case eGAMEOBJECT_Component_Animation:
                _component = new QeAnimation(key);
                break;
            case eGAMEOBJECT_Component_Plane:
                _component = new QePlane(key);
                break;
            case eGAMEOBJECT_Component_Cubemap:
                _component = new QeCubemap(key);
                break;
            case eGAMEOBJECT_Component_Partical:
                _component = new QeParticle(key);
                break;
            case eGAMEOBJECT_Component_Material:
                _component = new QeMaterial(key);
                break;
            case eGAMEOBJECT_Component_InputControl:
                _component = new QeInputControl(key);
                break;
            case eGAMEOBJECT_Component_RenderSetting:
                _component = new QeRenderSetting(key);
                break;
            case eGAMEOBJECT_Object:
                _component = new QeObject(key);
                break;
            case eGAMEOBJECT_Scene:
                _component = new QeScene(key);
                break;
        }
    }
    it = active_components.find(_type);
    if (it != active_components.end()) {
        it->second.push_back(_component);
    } else {
        active_components.insert(std::pair<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>(_type, {_component}));
    }
    _component->initialize(_property, _owner);

    return _component;
}

QeComponent *AeObjectManager::findComponent(AE_GAMEOBJECT_TYPE type, int _oid) {
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it = active_components.find(type);

    if (it != active_components.end()) {
        std::vector<QeComponent *>::iterator it1 = it->second.begin();
        while (it1 != it->second.end()) {
            if ((*it1)->data.oid == _oid) {
                return (*it1);
            }
            ++it1;
        }
        ++it;
    }
    return nullptr;
}

bool AeObjectManager::removeComponent(QeComponent *_component) {
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>::iterator it = active_components.find(_component->data.type);
    if (it != active_components.end()) {
        int index = findElementFromVector(it->second, _component);

        if (index == INDEX_NONE) return false;
        _component->clear();
        it->second.erase(it->second.begin() + index);

        it = idle_components.find(_component->data.type);
        if (it != idle_components.end()) {
            it->second.push_back(_component);
        } else {
            idle_components.insert(std::pair<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>>(_component->data.type, {_component}));
        }
        return true;
    }
    return false;
}
