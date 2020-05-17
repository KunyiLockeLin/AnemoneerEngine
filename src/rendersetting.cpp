#include "header.h"

void QeRenderSetting::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE

    GRAP->renderSetting = this;

    VkSampleCountFlags counts = std::min(VK->deviceProperties.limits.framebufferColorSampleCounts,
                                         VK->deviceProperties.limits.framebufferDepthSampleCounts);

    if (counts > uint32_t(component_data.msaa)) counts = component_data.msaa;
    if (counts >= VK_SAMPLE_COUNT_64_BIT)
        sampleCount = VK_SAMPLE_COUNT_64_BIT;
    else if (counts >= VK_SAMPLE_COUNT_32_BIT)
        sampleCount = VK_SAMPLE_COUNT_32_BIT;
    else if (counts >= VK_SAMPLE_COUNT_16_BIT)
        sampleCount = VK_SAMPLE_COUNT_16_BIT;
    else if (counts >= VK_SAMPLE_COUNT_8_BIT)
        sampleCount = VK_SAMPLE_COUNT_8_BIT;
    else if (counts >= VK_SAMPLE_COUNT_4_BIT)
        sampleCount = VK_SAMPLE_COUNT_4_BIT;
    else if (counts >= VK_SAMPLE_COUNT_2_BIT)
        sampleCount = VK_SAMPLE_COUNT_2_BIT;
    else
        sampleCount = VK_SAMPLE_COUNT_1_BIT;
    ENGINE->FPS = component_data.FPS;
    ENGINE->FPSTimer.setTimer(1000 / ENGINE->FPS);
}
