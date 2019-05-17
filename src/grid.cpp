#include "header.h"

void QeGrid::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    AST->getXMLfValue(&color.x, initProperty, 1, "colorR");
    AST->getXMLfValue(&color.y, initProperty, 1, "colorG");
    AST->getXMLfValue(&color.z, initProperty, 1, "colorB");
    AST->getXMLiValue(&column, initProperty, 1, "column");
    AST->getXMLiValue(&row, initProperty, 1, "row");

    float param[5];
    param[0] = color.x;
    param[1] = color.y;
    param[2] = color.z;
    param[3] = float(column);
    param[4] = float(row);

    modelData = AST->getModel("grid", false, param);

    AST->setGraphicsShader(graphicsShader, nullptr, "line");

    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeGrid::update1() { QeModel::update1(); }
