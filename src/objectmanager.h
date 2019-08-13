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

    std::map<QeComponentType, std::vector<QeComponent *>> components;
    std::map<QeComponentType, std::vector<QeComponent *>> unActiveComponents;

    QeComponent *spwanComponent(QeAssetXML *_property, QeObject *_owner = nullptr);
    QeComponent *findComponent(QeComponentType type, int _oid);
    bool removeComponent(QeComponent *component);

    void clear();
};
