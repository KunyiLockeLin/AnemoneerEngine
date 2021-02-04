#pragma once

#include "header.h"

class AeObjectManagerKey {
    friend class AeObjectManager;

   private:
    AeObjectManagerKey() {}
};

class AeObjectManager {
   private:
    AeObjectManagerKey key;

   public:
    AeObjectManager(AeGlobalKey &_key) {}
    ~AeObjectManager();

    void loadScene(int _eid);

    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>> active_components;
    std::map<AE_GAMEOBJECT_TYPE, std::vector<QeComponent *>> idle_components;

    QeComponent *spwanComponent(AeXMLNode *_property, QeObject *_owner = nullptr);
    QeComponent *findComponent(AE_GAMEOBJECT_TYPE type, int _oid);
    bool removeComponent(QeComponent *component);

    void clear();
};
