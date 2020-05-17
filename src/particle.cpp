#include "header.h"

void QeParticle::initialize(AeXMLNode *_property, QeObject *_owner) {
    COMPONENT_INITIALIZE

    bRotate = false;
    //particleRule = G_ENCODE->decodeParticle(initProperty);
    materialData = G_AST->getMaterialImage(component_data.image.c_str());

    VK->createBuffer(modelBuffer, sizeof(bufferData), nullptr);

    computePipeline.shader = G_AST->getShader(CONFIG->getXMLValue<std::string>("shaders.compute.particle.comp").c_str());

    shaderKey = "particle";
    G_AST->setGraphicsShader(graphicsShader, nullptr, shaderKey);

    // count
    totalParticlesSize = MATH->random<int>(component_data.count_total, component_data.count_range);
    currentParticlesSize = 0;  // particleRule->count_once;
    periodTimer.setTimer(component_data.count_period);
    particles.clear();
    bDeaths.resize(totalParticlesSize);
    memset(bDeaths.data(), 0, sizeof(bDeaths[0]) * bDeaths.size());

    // ahlpa
    graphicsPipeline.bAlpha = component_data.alpha;
    if (graphicsPipeline.bAlpha)
        GRAP->alphaModels.push_back(this);
    else
        GRAP->models.push_back(this);

    // size
    size.x = MATH->random<float>(component_data.size.x, component_data.size_range.x);
    size.y = MATH->random<float>(component_data.size.y, component_data.size_range.y);
    size.z = 1;

    bufferData.material = materialData->value;
    // bufferData.param2.y = (float)bornTargetTranformOID;

    VK->createBuffer(vertexBuffer, sizeof(QeVertex) * totalParticlesSize, nullptr);
    VK->createBuffer(outBuffer, sizeof(bDeaths[0]) * bDeaths.size(), (void *)bDeaths.data());

    VK->createDescriptorSet(descriptorSet);
    VK->updateDescriptorSet(&createDescriptorSetModel(), descriptorSet);

    VK->createDescriptorSet(descriptorSetCompute);
    QeDataDescriptorSetCompute descriptorSetData;
    descriptorSetData.texelBufferView = vertexBuffer.view;
    descriptorSetData.buffer = outBuffer.buffer;

    VK->updateDescriptorSet(&descriptorSetData, descriptorSetCompute);
}

void QeParticle::clear() {
    //delete particleRule;
    particles.clear();
    bDeaths.clear();
    vertexBuffer.~QeVKBuffer();
    outBuffer.~QeVKBuffer();

    QeModel::clear();
}

QeVertex QeParticle::createParticleData() {
    QeVertex particle;

    particle.pos.z = MATH->random<float>(component_data.init_pos_volume.z, component_data.init_pos_volume_range.z) *
                     (MATH->random<int>(0, 1) ? 1 : -1);
    int type = MATH->random<int>(0, 1);
    switch (type) {
        case 0:
            particle.pos.x = MATH->random<float>(component_data.init_pos_volume.x, component_data.init_pos_volume_range.x) *
                             (MATH->random<int>(0, 1) ? 1 : -1);
            particle.pos.y = MATH->random<float>(-(component_data.init_pos_volume.y + component_data.init_pos_volume_range.y),
                                                 (component_data.init_pos_volume.y + component_data.init_pos_volume_range.y) * 2);
            break;
        case 1:
            particle.pos.x = MATH->random<float>(-(component_data.init_pos_volume.x + component_data.init_pos_volume_range.x),
                                                 (component_data.init_pos_volume.x + component_data.init_pos_volume_range.x) * 2);
            particle.pos.y = MATH->random<float>(component_data.init_pos_volume.y, component_data.init_pos_volume_range.y) *
                             (MATH->random<int>(0, 1) ? 1 : -1);
            break;
    }
    if (component_data.init_pos_radius != 0 || component_data.init_pos_radius_range != 0) {
        float radius = MATH->random<float>(component_data.init_pos_radius, component_data.init_pos_radius_range);
        float radian =
            MATH->random<float>(component_data.init_pos_degree, component_data.init_pos_degree_range) * MATH->DEGREES_TO_RADIANS;
        particle.pos.x = radius * cos(radian);
        particle.pos.y = radius * sin(radian);
    }
    particle.pos.w = 0;
    // init pos = uv
    // particle.uv = particle.pos;

    // color
    particle.color.x = MATH->random<float>(component_data.color.x, component_data.color_range.x);
    particle.color.y = MATH->random<float>(component_data.color.y, component_data.color_range.y);
    particle.color.z = MATH->random<float>(component_data.color.z, component_data.color_range.z);
    particle.color.w = 1.0f;

    // normal = speed
    particle.normal.x = MATH->random<float>(component_data.init_speed.x, component_data.init_speed_range.x);
    particle.normal.y = MATH->random<float>(component_data.init_speed.y, component_data.init_speed_range.y);
    particle.normal.z = MATH->random<float>(component_data.init_speed.z, component_data.init_speed_range.z);

    // life time
    particle.normal.w = float(MATH->random<int>(component_data.life_second, component_data.life_range));

    // init speed & life time = joint
    // particle.joint = particle.normal;
    // particle.normal.w = 0;

    // tangent = force
    particle.tangent.x = MATH->random<float>(component_data.force.x, component_data.force_range.x);
    particle.tangent.y = MATH->random<float>(component_data.force.y, component_data.force_range.y);
    particle.tangent.z = MATH->random<float>(component_data.force.z, component_data.force_range.z);

    // reborn
    // particle.tangent.w = particleRule->bReborn;

    // rotate = texCoord
    // particles[i].texCoord.x = MATH->fRandom(particleRule->rotate_xy.x,
    // particleRule->rotate_xy.y); particles[i].texCoord.y =
    // MATH->fRandom(particleRule->rotate_xy.z, particleRule->rotate_xy.w);
    // particles[i].texCoord.z = MATH->fRandom(particleRule->rotate_z_color_r.x,
    // particleRule->rotate_z_color_r.y);

    // MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
    // MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
    // MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
    return particle;
}

QeDataDescriptorSetModel QeParticle::createDescriptorSetModel() {
    QeDataDescriptorSetModel descriptorSetData;
    descriptorSetData.modelBuffer = modelBuffer.buffer;
    bufferData.param1 = {0, 0, 0, 0};
    bufferData.param1.x = 1;
    descriptorSetData.baseColorMapImageView = materialData->image.pBaseColorMap->view;
    descriptorSetData.baseColorMapSampler = materialData->image.pBaseColorMap->sampler;
    return descriptorSetData;
}

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer &commandBuffer) {
    if (!currentParticlesSize) return;
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, eDescriptorSetLayout_Compute, 1,
                            &descriptorSetCompute.set, 0, nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->createComputePipeline(&computePipeline));
    vkCmdDispatch(commandBuffer, currentParticlesSize, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(QeDataDrawCommand *command) {
    if (!isShowByCulling(command->camera)) return;
    if (!currentParticlesSize) return;

    vkCmdBindDescriptorSets(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, eDescriptorSetLayout_Model,
                            1, &descriptorSet.set, 0, nullptr);

    graphicsPipeline.subpass = 0;
    graphicsPipeline.componentType = data.type;
    graphicsPipeline.sampleCount = GRAP->renderSetting->sampleCount;
    graphicsPipeline.renderPass = command->renderPass;
    graphicsPipeline.minorType = eGraphicsPipeLine_none;
    graphicsPipeline.shader = &graphicsShader;

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
    vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->createGraphicsPipeline(&graphicsPipeline));
    vkCmdDraw(command->commandBuffer, currentParticlesSize, 1, 0, 0);
}

void QeParticle::updatePreRender() {
    if (!totalParticlesSize) return;

    bool b = false;

    memcpy(bDeaths.data(), outBuffer.mapped, sizeof(bDeaths[0]) * bDeaths.size());

    if (vertexBuffer.mapped) memcpy(particles.data(), vertexBuffer.mapped, sizeof(particles[0]) * particles.size());

    for (int i1 = 0, i2 = 0; i2 < currentParticlesSize; ++i1, ++i2) {
        if (bDeaths[i1]) {
            b = true;
            particles.erase(particles.begin() + i2);
            --i2;
            --currentParticlesSize;
            if (!component_data.reborn) --totalParticlesSize;
        }
    }

    int passMilliSecond;
    if (periodTimer.checkTimer(passMilliSecond)) {
        int size = currentParticlesSize;
        size += component_data.count_once;
        if (size > totalParticlesSize) size = totalParticlesSize;
        if (size != currentParticlesSize) {
            b = true;
            for (int i = currentParticlesSize; i < size; ++i) {
                QeVertex particle = createParticleData();

                if (component_data.bornTargetTranformOID) {
                    QeTransform *target =
                        (QeTransform *)OBJMGR->findComponent(eGAMEOBJECT_Component_Transform, component_data.bornTargetTranformOID);
                    particle.pos.x += target->worldPosition().x;
                    particle.pos.y += target->worldPosition().y;
                    particle.pos.z += target->worldPosition().z;
                }
                particles.push_back(particle);
            }
            currentParticlesSize = size;
        }
    }

    if (b) {
        currentParticlesSize = int(particles.size());
        VK->setMemoryBuffer(vertexBuffer, sizeof(particles[0]) * particles.size(), particles.data());
        // memset(bDeaths.data(), 0, sizeof(bDeaths[0])*bDeaths.size());
        // VK->setMemoryBuffer(outBuffer, sizeof(bDeaths[0])*bDeaths.size(),
        // bDeaths.data());
    }

    AeVector<float, 3> scale = owner->transform->worldScale();
    scale.x *= size.x;
    scale.y *= size.y;

    bufferData.model = MATH->getTransformMatrix(owner->transform->worldPosition(), owner->transform->worldFaceEular(), scale,
                                                GRAP->getTargetCamera()->owner->transform->worldPosition());

    VK->setMemoryBuffer(modelBuffer, sizeof(bufferData), &bufferData);
}
