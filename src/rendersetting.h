#pragma once
#include "component.h"

class QeRenderSetting : public QeComponent {
   public:
    QeRenderSetting(AeObjectManagerKey &_key) : QeComponent(_key) {}
    ~QeRenderSetting() {}

    bool bNormal = false;
    bool bMesh = false;
    int msaa = 1;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
    float gamma = 2.2f;
    float exposure = 1.0f;
    float lineWidth = 2.0f;
    QeVector4f clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    virtual void initialize(QeAssetXML *_property, QeObject *_owner);
};
