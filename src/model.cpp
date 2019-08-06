#include "header.h"

void QeModel::initialize(QeAssetXML *_property, QeObject *_owner) {
    QeComponent::initialize(_property, _owner);

    AST->getXMLbValue(&graphicsPipeline.bAlpha, initProperty, 1, "alpha");
    AST->getXMLfValue(&bufferData.param2.x, initProperty, 1, "outlineWidth");

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    const char *c = AST->getXMLValue(initProperty, 1, "obj");
    modelData = AST->getModel(c);
    materialData = modelData->pMaterial;
    if (materialData) bufferData.material = materialData->value;

    bufferData.param2.y = (float)modelData->vertices.size();

    shaderKey = "model";
    AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);
    AST->setGraphicsShader(normalShader, nullptr, "normal");
    AST->setGraphicsShader(outlineShader, nullptr, "outline");

    AST->getXMLiValue(&materialOID, initProperty, 1, "materialOID");

    bUpdateMaterialOID = false;
    if (materialOID) bUpdateMaterialOID = true;
    graphicsPipeline.bAlpha = false;
    GRAP->models.push_back(this);

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
}

void QeModel::clear() {
    b2D = false;
    bRotate = true;
    graphicsShader = {nullptr, nullptr, nullptr, nullptr, nullptr};
    descriptorSet.~QeDataDescriptorSet();
    modelBuffer.~QeVKBuffer();

    if (graphicsPipeline.bAlpha)
        eraseElementFromVector<QeModel *>(GRAP->alphaModels, this);
    else
        eraseElementFromVector<QeModel *>(GRAP->models, this);
}

void QeModel::update1() {
    if (bUpdateMaterialOID && materialOID) {
        QeMaterial *material = (QeMaterial *)OBJMGR->findComponent(eComponent_material, materialOID);
        if (material) {
            bUpdateMaterialOID = false;

            if (graphicsPipeline.bAlpha != material->bAlpha) {
                graphicsPipeline.bAlpha = material->bAlpha;
                if (graphicsPipeline.bAlpha) {
                    eraseElementFromVector<QeModel *>(GRAP->models, this);
                    GRAP->alphaModels.push_back(this);
                } else {
                    eraseElementFromVector<QeModel *>(GRAP->alphaModels, this);
                    GRAP->models.push_back(this);
                }
            }
            materialData = &material->materialData;
            AST->setGraphicsShader(graphicsShader, material->initProperty, shaderKey);
            bufferData.material = materialData->value;
            VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);
        }
    }

    bufferData.model = owner->transform->worldTransformMatrix(bRotate);

    VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}
void QeModel::update2() {}

QeDataDescriptorSetModel QeModel::createDescriptorSetModel() {
    QeDataDescriptorSetModel descriptorSetData;
    descriptorSetData.modelBuffer = modelBuffer.buffer;

    bufferData.param1 = {0, 0, 0, 0};
    descriptorSetData.baseColorMapImageView = VK->emptyImage2D.view;
    descriptorSetData.baseColorMapSampler = VK->emptyImage2D.sampler;
    descriptorSetData.normalMapImageView = VK->emptyImage2D.view;
    descriptorSetData.normalMapSampler = VK->emptyImage2D.sampler;
    descriptorSetData.cubeMapImageView = VK->emptyImageCube.view;
    descriptorSetData.cubeMapSampler = VK->emptyImageCube.sampler;
    descriptorSetData.metallicRoughnessMapImageView = VK->emptyImage2D.view;
    descriptorSetData.metallicRoughnessMapSampler = VK->emptyImage2D.sampler;

    if (materialData) {
        if (materialData->image.pBaseColorMap) {
            descriptorSetData.baseColorMapImageView = materialData->image.pBaseColorMap->view;
            descriptorSetData.baseColorMapSampler = materialData->image.pBaseColorMap->sampler;
            bufferData.param1.x = 1;
        }
        if (materialData->image.pNormalMap) {
            descriptorSetData.normalMapImageView = materialData->image.pNormalMap->view;
            descriptorSetData.normalMapSampler = materialData->image.pNormalMap->sampler;
            bufferData.param1.y = 1;
        }
        if (materialData->image.pCubeMap) {
            descriptorSetData.cubeMapImageView = materialData->image.pCubeMap->view;
            descriptorSetData.cubeMapSampler = materialData->image.pCubeMap->sampler;
            bufferData.param1.z = 1;
        }
        if (materialData->image.pMetallicRoughnessMap) {
            descriptorSetData.metallicRoughnessMapImageView = materialData->image.pMetallicRoughnessMap->view;
            descriptorSetData.metallicRoughnessMapSampler = materialData->image.pMetallicRoughnessMap->sampler;
            bufferData.param1.w = 1;
        }
    }
    return descriptorSetData;
}

bool QeModel::isShowByCulling(QeCamera *camera) {
    if (componentType == eComponent_plane) {
        QePlane *plane = (QePlane *)this;
        if (plane->planeType == ePlane_2D) return true;
    }
    bool _bCullingShow = true;
    QeVector3f vec = owner->transform->worldPosition() - camera->owner->transform->worldPosition();
    float dis = MATH->length(vec);

    if (dis > camera->cullingDistance) return false;

    if (componentType != eComponent_cubemap && _bCullingShow) {
        float angle = MATH->getAnglefromVectors(camera->face(), vec);
        if (angle > 100 || angle < -100) return false;
    }

    return true;
}

void QeModel::updateDrawCommandBuffer(QeDataDrawCommand *command) {
    if (!isShowByCulling(command->camera)) return;

    vkCmdBindDescriptorSets(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, eDescriptorSetLayout_Model,
                            1, &descriptorSet.set, 0, nullptr);

    VkDeviceSize offsets[] = {0};

    graphicsPipeline.subpass = 0;
    graphicsPipeline.componentType = componentType;
    graphicsPipeline.sampleCount = GRAP->renderSetting->sampleCount;
    graphicsPipeline.renderPass = command->renderPass;
    graphicsPipeline.minorType = eGraphicsPipeLine_none;
    graphicsPipeline.shader = &graphicsShader;

    vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));

    switch (componentType) {
        case eComponent_model:
        case eComponent_cubemap:
        case eComponent_animation:
            vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
            vkCmdBindIndexBuffer(command->commandBuffer, modelData->index.buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
            break;

        case eComponent_plane:
            vkCmdDraw(command->commandBuffer, 1, 1, 0, 0);
            break;

        case eComponent_line:
        case eComponent_axis:
        case eComponent_grid:
            vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &modelData->vertex.buffer, offsets);
            vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
            break;

            // case eObject_Particle:
            //	break;
    }

    if (bufferData.param2.x && outlineShader.vert) {
        graphicsPipeline.minorType = eGraphicsPipeLine_stencilBuffer;
        graphicsPipeline.shader = &outlineShader;

        vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
        vkCmdDrawIndexed(command->commandBuffer, static_cast<uint32_t>(modelData->indexSize), 1, 0, 0, 0);
    }

    if (VK->bShowNormal && normalShader.vert) {
        graphicsPipeline.minorType = eGraphicsPipeLine_normal;
        graphicsPipeline.shader = &normalShader;

        vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
        vkCmdDraw(command->commandBuffer, uint32_t(modelData->vertices.size()), 1, 0, 0);
    }
}
