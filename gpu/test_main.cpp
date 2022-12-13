#include "gpu.h"
#include "ui.h"  // TODO: ui needs to be replaced with lib.ui when ilb.ui is completed.

// const char CONFIG_PATH[] = "data\\config.xml";
//#define CONFIG COM_MGR.getXML(CONFIG_PATH)

void draw_triangle() {
    UI->initialize();
    UI->window_title_ = "Test GPU: draw_triangle";

    ae::gpu::InitializeInfo initialize_info{};
    initialize_info.rendering_info_.present_info_.hinstance_ = UI->windowInstance;
    initialize_info.rendering_info_.present_info_.hwnd_ = UI->mainWindow;
    initialize_info.device_info_.layer_names_.push_back("VK_LAYER_KHRONOS_validation");

    ae::gpu::IDevice* device = nullptr;
    ae::gpu::initialize(initialize_info, device);

    ae::gpu::IRendering* rendering = device->get_rendering();

    while (!UI->closed) {
        ae::gpu::pre_update();
        UI->update1();
        ae::gpu::update();
        ae::gpu::post_update();
        UI->update2();
    }
    ae::gpu::cleanup();
}

enum TestList {
    TEST_DRAW_TRIANGLE,
    TEST_DRAW_CUBE,
};

TestList target_test = TEST_DRAW_TRIANGLE;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        LOGOBJ.setOutput(*CONFIG, "TestGPU_");
        switch (target_test) {
            case TEST_DRAW_TRIANGLE:
                draw_triangle();
                break;
            default:
                break;
        }
    } catch (const std::runtime_error& e) {
        LOG(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
