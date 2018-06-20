#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property, int _parentOID) {
	
	QePoint::init(_property,  _parentOID);

	particleRule = AST->getParticle(eid);
	mtlData = AST->getMaterialImage(particleRule->image);
	
	computeShader = AST->getShader(AST->getXMLValue(4, AST->CONFIG, "default", "computeShader", "particle"));
	AST->setGraphicsShader(shader, nullptr, "particle");

	// count
	particlesSize = MATH->iRandom(int(particleRule->count_total), int(particleRule->count_range));
	particles.resize(particlesSize);
	bDeaths.resize(particlesSize);
	memset( bDeaths.data(), 0, sizeof(bDeaths[0])*bDeaths.size());

	for (uint32_t i = 0; i < particlesSize; ++i) {

		// pos
		particles[i].pos.z = MATH->fRandom(particleRule->init_pos_square.z, particleRule->init_pos_square_range.z)*(MATH->iRandom(0, 1) ? 1 : -1);
		int type = MATH->iRandom(0, 1);
		switch (type) {
		case 0:
			particles[i].pos.x = MATH->fRandom(particleRule->init_pos_square.x, particleRule->init_pos_square_range.x)*(MATH->iRandom(0, 1) ? 1 : -1);
			particles[i].pos.y = MATH->fRandom(-(particleRule->init_pos_square.y + particleRule->init_pos_square_range.y), 
												(particleRule->init_pos_square.z + particleRule->init_pos_square_range.z) * 2);
			break;
		case 1:
			particles[i].pos.x = MATH->fRandom(-(particleRule->init_pos_square.x + particleRule->init_pos_square_range.x), 
												(particleRule->init_pos_square.x + particleRule->init_pos_square_range.x) * 2);
			particles[i].pos.y = MATH->fRandom(particleRule->init_pos_square.y, particleRule->init_pos_square_range.y)*(MATH->iRandom(0, 1) ? 1 : -1);
			break;
		}
		if (particleRule->init_pos_cycle.x != 0 || particleRule->init_pos_cycle_range.x != 0) {
			float radius = MATH->fRandom(particleRule->init_pos_cycle.x, particleRule->init_pos_cycle_range.x);
			float radian = MATH->fRandom(particleRule->init_pos_cycle.y, particleRule->init_pos_cycle_range.y) * MATH->DEGREES_TO_RADIANS;
			particles[i].pos.x = radius * cos(radian);
			particles[i].pos.y = radius * sin(radian);
		}
		particles[i].pos.w = 0;
		// init pos = uv
		particles[i].uv = particles[i].pos;

		// color
		particles[i].color.x = MATH->fRandom(particleRule->color.x, particleRule->color_range.x);
		particles[i].color.y = MATH->fRandom(particleRule->color.y, particleRule->color_range.y);
		particles[i].color.z = MATH->fRandom(particleRule->color.z, particleRule->color_range.z);
		particles[i].color.w = 1.0f;

		// normal = speed
		particles[i].normal.x = MATH->fRandom(particleRule->init_speed.x, particleRule->init_speed_range.x);
		particles[i].normal.y = MATH->fRandom(particleRule->init_speed.y, particleRule->init_speed_range.y);
		particles[i].normal.z = MATH->fRandom(particleRule->init_speed.z, particleRule->init_speed_range.z);

		// life time
		particles[i].normal.w = MATH->fRandom(particleRule->life_scend, particleRule->life_range);

		// init speed & life time = joint
		particles[i].joint = particles[i].normal;
		particles[i].normal.w = 0;

		// tangent = force
		particles[i].tangent.x = MATH->fRandom(particleRule->force.x, particleRule->force_range.x);
		particles[i].tangent.y = MATH->fRandom(particleRule->force.y, particleRule->force_range.y);
		particles[i].tangent.z = MATH->fRandom(particleRule->force.z, particleRule->force_range.z);

		// reborn
		particles[i].tangent.w = particleRule->bReborn;

		// rotate = texCoord
		//particles[i].texCoord.x = MATH->fRandom(particleRule->rotate_xy.x, particleRule->rotate_xy.y);
		//particles[i].texCoord.y = MATH->fRandom(particleRule->rotate_xy.z, particleRule->rotate_xy.w);
		//particles[i].texCoord.z = MATH->fRandom(particleRule->rotate_z_color_r.x, particleRule->rotate_z_color_r.y);

		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
		//MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
		//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
	}
	// ahlpa
	bAlpha = particleRule->bAlpha;

	// size
	size.x = MATH->fRandom(particleRule->size.x, particleRule->size_range.x);
	size.y = MATH->fRandom(particleRule->size.y, particleRule->size_range.y);
	size.z = 1;

	bufferData.material.baseColor = particles[0].color;
	bufferData.param.y = float(bFollowPos + 1);

	VK->createBuffer(vertexBuffer, sizeof(particles[0]) * particles.size(), (void*)particles.data());
	VK->createBuffer(outBuffer, sizeof(bDeaths[0]) * bDeaths.size(), (void*)bDeaths.data());
}

QeDataDescriptorSetModel QeParticle::createDescriptorSetModel(int index) {
	QeDataDescriptorSetModel descriptorSetData;
	descriptorSetData.modelBuffer = modelBuffer.buffer;
	descriptorSetData.baseColorMapImageViews = mtlData->image.pBaseColorMap->view;
	descriptorSetData.baseColorMapSamplers = mtlData->image.pBaseColorMap->sampler;
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

	if (!particlesSize) return;
	if (!particleRule->bReborn) {
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

	if (parentOID > 0) {
		QePoint* p = OBJMGR->getObject(parentOID);
		if (p) {
			QeMatrix4x4f mat = p->getAttachMatrix(attachSkeletonName.c_str());
			bufferData.model._30 += mat._30;
			bufferData.model._31 += mat._31;
			bufferData.model._32 += mat._32;
			//bufferData.model = p->getAttachMatrix(attachSkeletonName.c_str())*bufferData.model;
		}
	}
}