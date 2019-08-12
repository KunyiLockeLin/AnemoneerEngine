#include "header.h"

void QeRenderSetting::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);
    GRAP->renderSetting = this;
    bMesh = initProperty->getXMLValueb("mesh");
    bNormal = initProperty->getXMLValueb("normal");
    clearColor = initProperty->getXMLValueRGBA("clearColor");
    gamma = initProperty->getXMLValuef("gamma");
    exposure = initProperty->getXMLValuef("exposure");
    lineWidth = initProperty->getXMLValuef("lineWidth");
    msaa = initProperty->getXMLValuei("msaa");

    VkSampleCountFlags counts = std::min(VK->deviceProperties.limits.framebufferColorSampleCounts,
                                         VK->deviceProperties.limits.framebufferDepthSampleCounts);

    if (counts > uint32_t(msaa)) counts = msaa;
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
    QE->FPS = initProperty->getXMLValuei("FPS");
    QE->FPSTimer.setTimer(1000 / QE->FPS);
}
