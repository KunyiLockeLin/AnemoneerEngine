#pragma once

#include "header.h"

class QeQueenEngineKey {
    friend class QueenEngine;

   private:
    QeQueenEngineKey() {}
};

class QueenEngine {
   private:
    QeQueenEngineKey key;

   public:
    QueenEngine(QeGlobalKey &_key) {}
    //~QueenEngine() {}
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
