#include "header.h"

void QeGrid::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    float param[5];
    param[0] = component_data.color.x;
    param[1] = component_data.color.y;
    param[2] = component_data.color.z;
    param[3] = float(component_data.column);
    param[4] = float(component_data.row);

    modelData = G_AST.getModel("grid", false, param);

    G_AST.setGraphicsShader(graphicsShader, nullptr, "line");

    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeGrid::updatePreRender() { QeModel::updatePreRender(); }
