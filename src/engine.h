#pragma once

#include "header.h"

class AnemoneerEngine {
   public:
    AnemoneerEngine(AeGlobalKey &_key) {}
    //~AnemoneerEngine() {}
    void initialize();
    void run();

    bool bPause = false;
    bool bClosed = false;
    float deltaTime;
    int currentFPS = 0;
    int FPS = 0;
    QeTimer FPSTimer;

    void mainLoop();
};
