#pragma once
#include "header.h"

class QeRenderSetting : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(QeRenderSetting, AeGameObjectComponentRenderSettingData)

    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
};
