#pragma once

#include "header.h"

class AeGlobalKey {
    friend class AeGlobal;

   private:
    AeGlobalKey() {}
};

class AeGlobal {
   private:
    AeGlobal();
    AeGlobalKey key;

   public:
    ~AeGlobal();

    static AeGlobal &getInstance();
    AngryEngine *engine = nullptr;
    QeVulkan *vulkan = nullptr;
    AeUI *ui = nullptr;
    QeGraphics *graphics = nullptr;
    QeCommand *command = nullptr;
    AeObjectManager *objectmanager = nullptr;
    QeScene *scene = nullptr;
};

#define GLB AeGlobal::getInstance()
#define ENGINE GLB.engine
#define VK GLB.vulkan
#define UI GLB.ui
#define GRAP GLB.graphics
#define SCENE GLB.scene
#define OBJMGR GLB.objectmanager
#define CMD(msg) GLB.command->inputCommand(msg)

const char CONFIG_PATH[] = "data\\config.xml";
#define CONFIG AST->getXML(CONFIG_PATH)
