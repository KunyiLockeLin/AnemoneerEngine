#include "header.h"

AeGlobal::AeGlobal() {
    if (engine == nullptr) engine = new AngryEngine(key);
    if (ui == nullptr) ui = new AeUI(key);
    if (objectmanager == nullptr) objectmanager = new AeObjectManager(key);
    if (graphics == nullptr) graphics = new QeGraphics(key);
    if (vulkan == nullptr) vulkan = new QeVulkan(key);
    if (command == nullptr) command = new QeCommand(key);
}

AeGlobal::~AeGlobal() {
    if (command != nullptr) {
        delete command;
        command = nullptr;
    }
    if (graphics != nullptr) {
        delete graphics;
        graphics = nullptr;
    }
    if (vulkan != nullptr) {
        delete vulkan;
        vulkan = nullptr;
    }
    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }
    if (ui != nullptr) {
        delete ui;
        ui = nullptr;
    }
    if (engine != nullptr) {
        delete engine;
        engine = nullptr;
    }
}

AeGlobal &AeGlobal::getInstance() {
    static AeGlobal _s;
    return _s;
}
