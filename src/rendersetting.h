#pragma once
#include "header.h"

class QeRenderSetting : public QeComponent {
   public:
    COMPONENT_CLASS_DECLARE(RenderSetting)

    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
};
