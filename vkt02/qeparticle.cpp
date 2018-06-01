#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property) {
	
	initProperty = _property;
	
	const char * c = AST->getXMLValue(_property, 1, "paritcleid");
	id = atoi(c);

	c = AST->getXMLValue(_property, 1, "paritcleEid");
	eid = atoi(c);
	particleRule = AST->getParticle(c);

	AST->getXMLbValue(bFollow, *_property, 1, "paritclefollow");

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
			particles[i].pos.y = MATH->fRandom(-(particleRule->init_pos_lw.z+particleRule->init_pos_lw.w), (particleRule->init_pos_lw.z + particleRule->init_pos_lw.w)*2);
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
		// init pos = texCoord
		particles[i].texCoord = particles[i].pos;

		// color
		particles[i].color.x = MATH->fRandom(particleRule->rotate_z_color_r.z, particleRule->rotate_z_color_r.w);
		particles[i].color.y = MATH->fRandom(particleRule->color_gb.x, particleRule->color_gb.y);
		particles[i].color.z = MATH->fRandom(particleRule->color_gb.z, particleRule->color_gb.w);

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

		// size
		size.x = MATH->fRandom(particleRule->alpha_born_size_x.z, particleRule->alpha_born_size_x.w);
		size.y = MATH->fRandom(particleRule->size_y_init_pos_d.x, particleRule->size_y_init_pos_d.y);
		size.z = 1;
	
		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
		//MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
	}

	VK->createBufferData((void*)particles.data(), sizeof(particles[0]) * particles.size(), VertexBuffer.buffer, VertexBuffer.memory, &VertexBuffer.mapped);
	VK->createBufferView(VertexBuffer.buffer, VK_FORMAT_R32G32B32A32_SFLOAT, VertexBuffer.view);

	//modelData = AST->getModel("point");
	pMaterial = AST->getMaterialImage(particleRule->image);
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);
	ubo.param.z = float(bFollow+1);
	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);
	VK->createBuffer(sizeof(bDeaths[0])*bDeaths.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, outBuffer.buffer, outBuffer.memory );
	VK->setMemory(outBuffer.memory, bDeaths.data(), sizeof(bDeaths[0])*bDeaths.size(), &outBuffer.mapped);
	vkMapMemory(VK->device, outBuffer.memory, 0, sizeof(bDeaths[0])*bDeaths.size(), 0, &outBuffer.mapped);

	/*VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = outBuffer.memory;
	mappedRange.offset = 0;
	mappedRange.size = sizeof(bDeaths);
	vkFlushMappedMemoryRanges(VK->device, 1, &mappedRange);*/
	//vkInvalidateMappedMemoryRanges(VK->device, 1, &mappedRange);
	//VK->createUniformBuffer(sizeof(QeAssetParticleRule), uboParticleRule.buffer, uboParticleRule.memory);
	//VK->setMemory(uboParticleRule.memory, (void*)&particleRule, sizeof(particleRule));
	
	QeDataDescriptorSet data;
	data.uboBuffer = uboBuffer.buffer;
	data.materialBuffer = pMaterial->uboBuffer.buffer;
	data.inputStorageTexeLBufferView = VertexBuffer.view;
	data.outputStorageTexeLBufferView = VertexBuffer.view;
	data.diffuseMapImageViews = pMaterial->image.pDiffuseMap->view;
	data.diffueMapSamplers = pMaterial->image.pDiffuseMap->sampler;
	data.outputBuffer = outBuffer.buffer;
	VK->updateDescriptorSet(data, descriptorSet);

	computeShader = AST->getShader(AST->getXMLValue(4, AST->CONFIG, "defaultShader", "particle", "comp"));
	AST->setShader(shader, nullptr, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "particle"));

	createPipeline();

	// attach
	c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	attachID = atoi(c);
	attachSkeletonName = AST->getXMLValue(_property, 1, "paritcleSkeleton");
}

void QeParticle::createPipeline() {

	graphicsPipeline = VK->createGraphicsPipeline(&shader, ePipeLine_Point, bAlpha );
	computePipeline = VK->createComputePipeline(computeShader);
}

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer) {

	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdDispatch(computeCommandBuffer, particlesSize, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//if (!bShow || !bCullingShow) return;
	
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &VertexBuffer.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, particlesSize, 1, 0, 0);
	
}

void QeParticle::updateCompute(float time) {
	VK->pushConstants[0] = time;// float(timer.getPassTime());
	//void* buf;
	//vkMapMemory(VK->device, outBuffer.memory, 0, sizeof(bDeaths[0])*bDeaths.size(), 0, &buf);
	memcpy(bDeaths.data(), outBuffer.mapped, sizeof(bDeaths[0])*bDeaths.size());
	//vkUnmapMemory(VK->device, outBuffer.memory);
	//memcpy(bDeaths.data(), outBuffer.memory, sizeof(char)*particlesSize);
	VP->bUpdateDrawCommandBuffers = true;
}

void QeParticle::setMatModel() {

	QeMatrix4x4f mat;

	mat *= MATH->translate(pos);
	mat *= MATH->scale(size);

	ubo.model = mat;

	if (attachID > 0) {
		QeBase* base = OBJMGR->getPoint(attachID, nullptr);
		if (base) {
			ubo.model._30 += base->pos.x;
			ubo.model._31 += base->pos.y;
			ubo.model._32 += base->pos.z;
		}
		else {
			QeModel* model = OBJMGR->getModel(attachID, nullptr);
			if (model != nullptr) {
				QeMatrix4x4f mat = model->getAttachMatrix(attachSkeletonName);
				ubo.model._30 += mat._30;
				ubo.model._31 += mat._31;
				ubo.model._32 += mat._32;
			}
		}
	}
}