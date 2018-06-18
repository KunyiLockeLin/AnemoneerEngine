#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property) {
	
	initProperty = _property;

	AST->getXMLuiValue(id, *initProperty, 1, "paritcleid");

	eid = AST->getXMLValue(initProperty, 1, "paritcleEid");
	particleRule = AST->getParticle(eid.c_str());

	AST->getXMLbValue(bFollow, *_property, 1, "paritclefollow");

	pMaterial = AST->getMaterialImage(particleRule->image);
	
	computeShader = AST->getShader(AST->getXMLValue(3, AST->CONFIG, "computeShader", "particle"));
	AST->setShader(shader, nullptr, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "particle"));

	setProperty();

	// count
	particlesSize = MATH->iRandom(int(particleRule->count_life.x), int(particleRule->count_life.y));
	particles.resize(particlesSize);
	bDeaths.resize(particlesSize);
	memset( bDeaths.data(), 0, sizeof(bDeaths[0])*bDeaths.size());

	for (uint32_t i = 0; i < particlesSize; ++i) {

		// pos
		particles[i].pos.z = MATH->fRandom(particleRule->init_pos_hr.x, particleRule->init_pos_hr.y)*(MATH->iRandom(0, 1) ? 1 : -1);
		int type = MATH->iRandom(0, 1);
		switch (type) {
		case 0:
			particles[i].pos.x = MATH->fRandom(particleRule->init_pos_lw.x, particleRule->init_pos_lw.y)*(MATH->iRandom(0, 1) ? 1 : -1);
			particles[i].pos.y = MATH->fRandom(-(particleRule->init_pos_lw.z + particleRule->init_pos_lw.w), (particleRule->init_pos_lw.z + particleRule->init_pos_lw.w) * 2);
			break;
		case 1:
			particles[i].pos.x = MATH->fRandom(-(particleRule->init_pos_lw.x + particleRule->init_pos_lw.y), (particleRule->init_pos_lw.x + particleRule->init_pos_lw.y) * 2);
			particles[i].pos.y = MATH->fRandom(particleRule->init_pos_lw.z, particleRule->init_pos_lw.w)*(MATH->iRandom(0, 1) ? 1 : -1);
			break;
		}
		if (particleRule->init_pos_hr.z != 0 || particleRule->init_pos_hr.w != 0) {
			float radius = MATH->fRandom(particleRule->init_pos_hr.z, particleRule->init_pos_hr.w);
			float radian = MATH->fRandom(particleRule->size_y_init_pos_d.z, particleRule->size_y_init_pos_d.w) * MATH->DEGREES_TO_RADIANS;
			particles[i].pos.x = radius * cos(radian);
			particles[i].pos.y = radius * sin(radian);
		}
		particles[i].pos.w = 0;
		// init pos = uv
		particles[i].uv = particles[i].pos;

		// color
		particles[i].color.x = MATH->fRandom(particleRule->rotate_z_color_r.z, particleRule->rotate_z_color_r.w);
		particles[i].color.y = MATH->fRandom(particleRule->color_gb.x, particleRule->color_gb.y);
		particles[i].color.z = MATH->fRandom(particleRule->color_gb.z, particleRule->color_gb.w);
		particles[i].color.w = 1.0f;

		// ahlpa
		bAlpha = particleRule->alpha_born_size_x.x;

		// normal = speed
		particles[i].normal.x = MATH->fRandom(particleRule->init_speed_xy.x, particleRule->init_speed_xy.y);
		particles[i].normal.y = MATH->fRandom(particleRule->init_speed_xy.z, particleRule->init_speed_xy.w);
		particles[i].normal.z = MATH->fRandom(particleRule->init_speed_z_force_x.x, particleRule->init_speed_z_force_x.y);

		// life time
		particles[i].normal.w = MATH->fRandom(particleRule->count_life.z, particleRule->count_life.w);

		// init speed & life time = joint
		particles[i].joint = particles[i].normal;
		particles[i].normal.w = 0;

		// tangent = force
		particles[i].tangent.x = MATH->fRandom(particleRule->init_speed_z_force_x.z, particleRule->init_speed_z_force_x.w);
		particles[i].tangent.y = MATH->fRandom(particleRule->force_yz.x, particleRule->force_yz.y);
		particles[i].tangent.z = MATH->fRandom(particleRule->force_yz.z, particleRule->force_yz.w);

		// reborn
		particles[i].tangent.w = particleRule->alpha_born_size_x.y;

		// rotate = texCoord
		//particles[i].texCoord.x = MATH->fRandom(particleRule->rotate_xy.x, particleRule->rotate_xy.y);
		//particles[i].texCoord.y = MATH->fRandom(particleRule->rotate_xy.z, particleRule->rotate_xy.w);
		//particles[i].texCoord.z = MATH->fRandom(particleRule->rotate_z_color_r.x, particleRule->rotate_z_color_r.y);

		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
		//MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
	}
	// size
	size.x = MATH->fRandom(particleRule->alpha_born_size_x.z, particleRule->alpha_born_size_x.w);
	size.y = MATH->fRandom(particleRule->size_y_init_pos_d.x, particleRule->size_y_init_pos_d.y);
	size.z = 1;

	bufferData.material.baseColor = particles[0].color;
	bufferData.param.y = float(bFollow + 1);

	VK->createBuffer(vertexBuffer, sizeof(particles[0]) * particles.size(), (void*)particles.data());
	VK->createBuffer(outBuffer, sizeof(bDeaths[0]) * bDeaths.size(), (void*)bDeaths.data());
	AST->getXMLiValue(attachID, *initProperty, 1, "id");
	attachSkeletonName = AST->getXMLValue(initProperty, 1, "paritcleSkeleton");
	pos = { 0,0,0 };
}

QeDataDescriptorSetModel QeParticle::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageViews = pMaterial->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = pMaterial->image.pBaseColorMap->sampler;
	descriptorSetData.texelBufferView = vertexBuffer.view;
	descriptorSetData.computeShaderoutputBuffer = outBuffer.buffer;
	return descriptorSetData;
}

void QeParticle::createPipeline() {

	if (particlesSize == 0) return;
	graphicsPipeline = VK->createGraphicsPipeline(&shader, eGraphicsPipeLine_Point, bAlpha );
	computePipeline = VK->createComputePipeline(computeShader);
}

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer) {

	if (particlesSize == 0) return;
	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(0);
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdDispatch(computeCommandBuffer, particlesSize, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//if (!bShow || !bCullingShow) return;
	if (particlesSize == 0) return;

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, particlesSize, 1, 0, 0);
}

void QeParticle::updateCompute(float time) {

	if (particleRule->alpha_born_size_x.y == 0) {
		memcpy(bDeaths.data(), outBuffer.mapped, sizeof(bDeaths[0])*bDeaths.size());
		memcpy(particles.data(), vertexBuffer.mapped, sizeof(particles[0])*particles.size());

		size_t size = bDeaths.size();
		bool b = false;
		for (int i = 0; i < size; ++i) {
			if (bDeaths[i]) {
				b = true;
				particles.erase(particles.begin() + i);
				bDeaths.erase(bDeaths.begin() + i);
				--i;
				--size;
			}
		}

		if (b) {
			particlesSize = uint32_t(particles.size());
			VK->setMemoryBuffer(vertexBuffer, sizeof(particles[0])*particles.size(), particles.data());
			VK->setMemoryBuffer(outBuffer, sizeof(bDeaths[0])*bDeaths.size(), bDeaths.data());
			VP->bUpdateDrawCommandBuffers = true;
		}
	}
	updateBuffer();
}

void QeParticle::setMatModel() {

	if (particlesSize == 0) return;

	QeMatrix4x4f mat;

	mat *= MATH->translate(pos);
	mat *= MATH->scale(size);

	bufferData.model = mat;

	if (attachID > 0) {
		QeBase* base = OBJMGR->getPoint(attachID, nullptr);
		if (base) {
			bufferData.model._30 += base->pos.x;
			bufferData.model._31 += base->pos.y;
			bufferData.model._32 += base->pos.z;
		}
		else {
			QeModel* model = OBJMGR->getModel(attachID, nullptr);
			if (model != nullptr) {
				QeMatrix4x4f mat = model->getAttachMatrix(attachSkeletonName);
				bufferData.model._30 += mat._30;
				bufferData.model._31 += mat._31;
				bufferData.model._32 += mat._32;
			}
		}
	}
}