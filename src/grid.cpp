#include "header.h"

void QeGrid::initialize(AeXMLNode *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    color = initProperty->getXMLValueRGB("color");
    column = initProperty->getXMLValuei("column");
    row = initProperty->getXMLValuei("row");

    float param[5];
    param[0] = color.x;
    param[1] = color.y;
    param[2] = color.z;
    param[3] = float(column);
    param[4] = float(row);

    modelData = G_AST->getModel("grid", false, param);

    G_AST->setGraphicsShader(graphicsShader, nullptr, "line");

    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeGrid::update1() { QeModel::update1(); }
