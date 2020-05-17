#include "header.h"

void AngryEngine::run() {
    bClosed = false;

    if (CONFIG->getXMLValue<bool>("setting.environment.outputLog")) {
        std::string outputPath = CONFIG->getXMLValue<std::string>("setting.path.log");
        outputPath += "Ae";
        LOGOBJ->switchOutput(true, outputPath.c_str());
    }
    UI->initialize();
    VK->initialize();
    initialize();
    mainLoop();
}

void AngryEngine::initialize() {
    UI->resizeAll();
    OBJMGR->loadScene(CONFIG->getXMLValue<int>("setting.environment.currentSceneEID"));
}

void AngryEngine::mainLoop() {
    while (!bClosed) {
        int passMilliSecond;
        if (FPSTimer.checkTimer(passMilliSecond)) {
            currentFPS = 1000 / passMilliSecond;
            deltaTime = float(passMilliSecond) / 1000;

            UI->update1();
            if (!bPause) {
                VK->update1();
                GRAP->update1();
                SCENE->updatePreRender();

                SCENE->updatePostRedner();
                // OBJMGR->update2();
                GRAP->update2();
                VK->update2();

                UI->update2();
            }
        }
    }
    vkDeviceWaitIdle(VK->device);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        ENGINE->run();
    } catch (const std::runtime_error &e) {
        LOG(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
