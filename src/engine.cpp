#include "header.h"

void QueenEngine::run() {
    bClosed = false;

    WIN->initialize();
    VK->initialize();

    initialize();
    mainLoop();
}

void QueenEngine::initialize() {
    WIN->resizeAll();
    SCENE->loadScene(CONFIG->getXMLValuei("setting.environment.currentSceneEID"));
}

void QueenEngine::mainLoop() {
    while (!bClosed) {
        int passMilliSecond;
        if (FPSTimer.checkTimer(passMilliSecond)) {
            currentFPS = 1000 / passMilliSecond;
            deltaTime = float(passMilliSecond) / 1000;

            WIN->update1();
            if (!bPause) {
                VK->update1();
                GRAP->update1();
                SCENE->update1();

                SCENE->update2();
                // OBJMGR->update2();
                GRAP->update2();
                VK->update2();

                WIN->update2();
            }
        }
    }
    vkDeviceWaitIdle(VK->device);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        QE->run();
    } catch (const std::runtime_error &e) {
        LOG(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
