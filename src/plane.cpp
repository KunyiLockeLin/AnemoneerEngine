#include "header.h"

void QePlane::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    modelData = nullptr;
    materialData = nullptr;
    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    planeType = (QePlaneType)initProperty->getXMLValuei("planeType");
    materialOID = initProperty->getXMLValuei("materialOID");
    targetCameraOID = initProperty->getXMLValuei("targetCameraOID");

    bUpdateTargetCameraOID = false;
    if (targetCameraOID) bUpdateTargetCameraOID = true;

    bUpdateMaterialOID = false;
    if (materialOID) bUpdateMaterialOID = true;
    graphicsPipeline.bAlpha = false;

    switch (planeType) {
        case ePlane_3D:
            shaderKey = "plane";
            bRotate = true;
            GRAP->models.push_back(this);
            break;
        case ePlane_billboard:
            shaderKey = "billboard";
            bRotate = false;
            GRAP->models.push_back(this);
            break;
        case ePlane_2D:
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
    if (planeType == ePlane_2D) eraseElementFromVector<QeModel *>(GRAP->models2D, this);
}

QeDataDescriptorSetModel QePlane::createDescriptorSetModel() {
    if (targetCameraOID) {
        QeCamera *camera = (QeCamera *)OBJMGR->findComponent(eComponent_camera, targetCameraOID);

        if (camera) {
            QeDataDescriptorSetModel descriptorSetData;
            descriptorSetData.modelBuffer = modelBuffer.buffer;
            QeDataRender *render = GRAP->getRender(eRender_color, targetCameraOID);

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

void QePlane::update1() {
    if (bUpdateTargetCameraOID) {
        VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
        bUpdateTargetCameraOID = true;

        QeAssetXML *node = CONFIG->getXMLNode("shaders.graphics.render");
        G_AST->setGraphicsShader(graphicsShader, node, shaderKey);
    }

    if (targetCameraOID) {
        QeCamera *camera = (QeCamera *)OBJMGR->findComponent(eComponent_camera, targetCameraOID);

        if (camera) {
            QeVector3f scale = owner->transform->worldScale();
            scale.x *= MATH->fastSqrt((float(camera->renderSize.width) / camera->renderSize.height));
            bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(),
                                                        scale, GRAP->getTargetCamera()->owner->transform->worldPosition());
            VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
        }
    } else {
        QeModel::update1();
    }
}
