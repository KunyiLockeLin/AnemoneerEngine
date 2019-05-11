#include "qeheader.h"

void QeCubemap::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    modelData = AST->getModel("cube", true);
    const char *image = AST->getXMLValue(initProperty, 1, "image");
    materialData = AST->getMaterialImage(image, true);

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);
    bufferData.material = materialData->value;
    shaderKey = "cubemap";
    AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);
    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

/*QeDataDescriptorSetModel QeCubemap::createDescriptorSetModel() {
        QeDataDescriptorSetModel descriptorSetData;
        descriptorSetData.modelBuffer = modelBuffer.buffer;
        bufferData.param = { 0,0,0 };
        descriptorSetData.cubeMapImageView = materialData->image.pCubeMap->view;
        descriptorSetData.cubeMapSampler =
materialData->image.pCubeMap->sampler; bufferData.param.z = 1;

        return descriptorSetData;
}*/
