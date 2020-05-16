#include "header.h"

void QeLine::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE_PARENT(QeModel)

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    modelData = G_AST->getModel("line", false, (float *)&component_data.color);

    shaderKey = "line";
    G_AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeLine::updatePreRender() {
    if (component_data.targetTransformOID == 0) return;

    QeTransform *targetTransform =
        (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.targetTransformOID);
    if (targetTransform == 0) return;

    AeVector<float,3> targetPos = targetTransform->worldPosition();
    AeVector<float, 3> pos = owner->transform->worldPosition();

     AeVector<float, 3> vec = targetPos - pos;
    float size = MATH->fastSqrt(MATH->length(vec));
     AeVector<float, 3> scale = {size, size, size};

    // LOG("line x: " + vec.x + "  y: " + vec.y + "  z: " + vec.z);
    bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), MATH->vectorToEulerAngles(vec), scale,
                                                GRAP->getTargetCamera()->owner->transform->worldPosition());

    VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}

/*QeDataDescriptorSetModel QeLine::createDescriptorSetModel() {
        QeDataDescriptorSetModel descriptorSetData;
        descriptorSetData.modelBuffer = modelBuffer.buffer;
        return descriptorSetData;
}*/
