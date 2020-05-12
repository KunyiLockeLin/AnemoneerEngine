#include "header.h"

void QeAxis::initialize(AeXMLNode *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    modelData = G_AST->getModel("axis");
    shaderKey = "line";
    G_AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeAxis::update1() {
    bufferData.model = owner->transform->worldTransformMatrix(true, true);
    VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}
