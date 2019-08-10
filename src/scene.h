#pragma once

#include "header.h"

class QeSceneManagerKey {
    friend class QeSceneManager;

   private:
    QeSceneManagerKey() {}
};

class QeSceneManager {
   private:
    QeSceneManagerKey key;

   public:
    QeSceneManager(QeGlobalKey &_key) {}
    ~QeSceneManager();

    int eid;
    std::string name;

    QeAssetXML *initProperty = nullptr;
    std::vector<QeObject *> scene_objects;

    void initialize(QeAssetXML *_property);
    void clearScene();
    void update1();
    void update2();

    void loadScene(int _eid);

    std::vector<QeObject *> objects;
    std::vector<QeObject *> unActiveObjects;

    std::map<QeComponentType, std::vector<QeComponent *>> components;
    std::map<QeComponentType, std::vector<QeComponent *>> unActiveComponents;

    QeObject *spwanObject(QeAssetXML *_property, QeObject *_parent = nullptr);
    QeComponent *spwanComponent(QeAssetXML *_property, QeObject *_owner = nullptr);
    QeObject *findObject(int _oid);
    QeObject *findObject(std::string name);
    QeComponent *findComponent(QeComponentType type, int _oid);
    bool removeObject(QeObject *_object);
    bool removeComponent(QeComponent *component);

    void clear();
};
