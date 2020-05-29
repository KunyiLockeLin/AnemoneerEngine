#include "ui.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        if (CONFIG->getXMLValue<bool>("setting.environment.outputLog")) {
            std::string outputPath = CONFIG->getXMLValue<std::string>("setting.path.log");
            outputPath += "testUI";
            LOGOBJ.switchOutput(true, outputPath.c_str());
        }
        UI_MGR.initialize();
        while (1) {
            UI_MGR.update1();
            UI_MGR.update2();
        }
    } catch (const std::runtime_error &e) {
        LOG(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
