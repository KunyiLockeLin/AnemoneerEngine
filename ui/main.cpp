#include "ui.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        LOGOBJ.setOutput(*CONFIG, "testUI_");
        UI_MGR.initialize();
        UI_MGR.loadUISet(CONFIG->getXMLValue<ID>("setting.environment.currentUISetEID"));
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
