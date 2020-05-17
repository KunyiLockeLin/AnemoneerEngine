#include "header.h"

void QePlane::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE_PARENT(QeModel)

    modelData = nullptr;
    materialData = nullptr;
    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    bUpdateTargetCameraOID = false;
    if (component_data.targetCameraOID) bUpdateTargetCameraOID = true;

    bUpdateMaterialOID = false;
    if (component_data.materialOID) bUpdateMaterialOID = true;
    graphicsPipeline.bAlpha = false;

    switch (component_data.planeType) {
        case ePLANE_3D:
            shaderKey = "plane";
            bRotate = true;
            GRAP->models.push_back(this);
            break;
        case ePLANE_Billboard:
            shaderKey = "billboard";
            bRotate = false;
            GRAP->models.push_back(this);
            break;
        case ePLANE_2D:
            shaderKey = "b2d";
            bRotate = false;
            GRAP->add2DModel(this);
            break;
    }

    G_AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QePlane::clear() {
    QeModel::clear();
    if (component_data.planeType == ePLANE_2D) eraseElementFromVector<QeModel *>(GRAP->models2D, this);
}

QeDataDescriptorSetModel QePlane::createDescriptorSetModel() {
    if (component_data.targetCameraOID) {
        QeCamera *camera = (QeCamera *)OBJMGR->findComponent(eGAMEOBJECT_Component_Camera, component_data.targetCameraOID);

        if (camera) {
            QeDataDescriptorSetModel descriptorSetData;
            descriptorSetData.modelBuffer = modelBuffer.buffer;
            QeDataRender *render = GRAP->getRender(eRENDER_Color, component_data.targetCameraOID);

            //if (render->subpass.size() > 0)
            //    descriptorSet.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            //else
            //    descriptorSet.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            descriptorSet.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            bufferData.param1 = {0, 0, 0, 0};
            descriptorSetData.normalMapImageView = VK->emptyImage2D.view;
            descriptorSetData.normalMapSampler = VK->emptyImage2D.sampler;
            descriptorSetData.cubeMapImageView = VK->emptyImageCube.view;
            descriptorSetData.cubeMapSampler = VK->emptyImageCube.sampler;

            descriptorSetData.baseColorMapImageView = render->colorImage.view;
            descriptorSetData.baseColorMapSampler = render->colorImage.sampler;
            bufferData.param1.x = 1;

            return descriptorSetData;
        }
    }
    return QeModel::createDescriptorSetModel();
}

void QePlane::updatePreRender() {
    if (bUpdateTargetCameraOID) {
        VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
        bUpdateTargetCameraOID = true;

        AeXMLNode *node = CONFIG->getXMLNode("shaders.graphics.render");
        G_AST->setGraphicsShader(graphicsShader, node, shaderKey);
    }

    if (component_data.targetCameraOID) {
        QeCamera *camera = (QeCamera *)OBJMGR->findComponent(eGAMEOBJECT_Component_Camera, component_data.targetCameraOID);

        if (camera) {
            AeVector<float, 3> scale = owner->transform->worldScale();
            scale.x *= MATH->fastSqrt((float(camera->component_data.renderSize.width) / camera->component_data.renderSize.height));
            bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(),
                                                        scale, GRAP->getTargetCamera()->owner->transform->worldPosition());
            VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
        }
    } else {
        QeModel::updatePreRender();
    }
}
