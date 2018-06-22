#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property, int _parentOID) {
	
	QePoint::init(_property,  _parentOID);

	particleRule = AST->getParticle(eid);
	mtlData = AST->getMaterialImage(particleRule->image);
	
	computeShader = AST->getShader(AST->getXMLValue(4, AST->CONFIG, "default", "computeShader", "particle"));
	AST->setGraphicsShader(graphicsShader, nullptr, "particle");

	// count
	totalParticlesSize = MATH->iRandom(particleRule->count_total, particleRule->count_range);
	currentParticlesSize = 0;// particleRule->count_once;
	periodTimer.setTimer(particleRule->count_period);
	particles.clear();
	bDeaths.resize(totalParticlesSize);
	memset( bDeaths.data(), 0, sizeof(bDeaths[0])*bDeaths.size());

	// ahlpa
	bAlpha = particleRule->bAlpha;

	// size
	size.x = MATH->fRandom(particleRule->size.x, particleRule->size_range.x);
	size.y = MATH->fRandom(particleRule->size.y, particleRule->size_range.y);
	size.z = 1;

	bufferData.material = mtlData->value;
	bufferData.param.x = bFollowPos;

	VK->createBuffer(vertexBuffer, sizeof(QeVertex) * totalParticlesSize, nullptr);
	VK->createBuffer(outBuffer, sizeof(bDeaths[0]) * bDeaths.size(), (void*)bDeaths.data());
}

QeVertex QeParticle::createParticleData() {

	QeVertex particle;

	particle.pos.z = MATH->fRandom(particleRule->init_pos_square.z, particleRule->init_pos_square_range.z)*(MATH->iRandom(0, 1) ? 1 : -1);
	int type = MATH->iRandom(0, 1);
	switch (type) {
	case 0:
		particle.pos.x = MATH->fRandom(particleRule->init_pos_square.x, particleRule->init_pos_square_range.x)*(MATH->iRandom(0, 1) ? 1 : -1);
		particle.pos.y = MATH->fRandom(-(particleRule->init_pos_square.y + particleRule->init_pos_square_range.y),
			(particleRule->init_pos_square.y + particleRule->init_pos_square_range.y) * 2);
		break;
	case 1:
		particle.pos.x = MATH->fRandom(-(particleRule->init_pos_square.x + particleRule->init_pos_square_range.x),
			(particleRule->init_pos_square.x + particleRule->init_pos_square_range.x) * 2);
		particle.pos.y = MATH->fRandom(particleRule->init_pos_square.y, particleRule->init_pos_square_range.y)*(MATH->iRandom(0, 1) ? 1 : -1);
		break;
	}
	if (particleRule->init_pos_cycle.x != 0 || particleRule->init_pos_cycle_range.x != 0) {
		float radius = MATH->fRandom(particleRule->init_pos_cycle.x, particleRule->init_pos_cycle_range.x);
		float radian = MATH->fRandom(particleRule->init_pos_cycle.y, particleRule->init_pos_cycle_range.y) * MATH->DEGREES_TO_RADIANS;
		particle.pos.x = radius * cos(radian);
		particle.pos.y = radius * sin(radian);
	}
	particle.pos.w = 0;
	// init pos = uv
	//particle.uv = particle.pos;

	// color
	particle.color.x = MATH->fRandom(particleRule->color.x, particleRule->color_range.x);
	particle.color.y = MATH->fRandom(particleRule->color.y, particleRule->color_range.y);
	particle.color.z = MATH->fRandom(particleRule->color.z, particleRule->color_range.z);
	particle.color.w = 1.0f;

	// normal = speed
	particle.normal.x = MATH->fRandom(particleRule->init_speed.x, particleRule->init_speed_range.x);
	particle.normal.y = MATH->fRandom(particleRule->init_speed.y, particleRule->init_speed_range.y);
	particle.normal.z = MATH->fRandom(particleRule->init_speed.z, particleRule->init_speed_range.z);

	// life time
	particle.normal.w = float(MATH->iRandom(particleRule->life_scend, particleRule->life_range));

	// init speed & life time = joint
	//particle.joint = particle.normal;
	//particle.normal.w = 0;

	// tangent = force
	particle.tangent.x = MATH->fRandom(particleRule->force.x, particleRule->force_range.x);
	particle.tangent.y = MATH->fRandom(particleRule->force.y, particleRule->force_range.y);
	particle.tangent.z = MATH->fRandom(particleRule->force.z, particleRule->force_range.z);

	// reborn
	//particle.tangent.w = particleRule->bReborn;

	// rotate = texCoord
	//particles[i].texCoord.x = MATH->fRandom(particleRule->rotate_xy.x, particleRule->rotate_xy.y);
	//particles[i].texCoord.y = MATH->fRandom(particleRule->rotate_xy.z, particleRule->rotate_xy.w);
	//particles[i].texCoord.z = MATH->fRandom(particleRule->rotate_z_color_r.x, particleRule->rotate_z_color_r.y);

	//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
	//MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
	//MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
	return particle;
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

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer& computeCommandBuffer) {

	if (!currentParticlesSize) return;
	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(0);
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdDispatch(computeCommandBuffer, currentParticlesSize, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//if (!bShow || !bCullingShow) return;
	if (!currentParticlesSize) return;

	std::vector<VkDescriptorSet> descriptorSets = getDescriptorSets(VP->currentCommandViewport);
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, uint32_t(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, currentParticlesSize, 1, 0, 0);
}

void QeParticle::updateCompute(float time) {

	if (!totalParticlesSize) return;
	
	bool b = false;

	memcpy(bDeaths.data(), outBuffer.mapped, sizeof(bDeaths[0])*bDeaths.size());

	if(vertexBuffer.mapped)
		memcpy(particles.data(), vertexBuffer.mapped, sizeof(particles[0])*particles.size());

	for (int i1 = 0, i2=0; i2 < currentParticlesSize; ++i1, ++i2) {
		if (bDeaths[i1]) {
			b = true;
			particles.erase(particles.begin() + i2);
			--i2;
			--currentParticlesSize;
			if (!particleRule->bReborn) --totalParticlesSize;
		}
	}

	int passMilliSecond;
	if (periodTimer.checkTimer(passMilliSecond)) {
		int size = currentParticlesSize;
		size += particleRule->count_once;
		if (size > totalParticlesSize) size = totalParticlesSize;
		if (size != currentParticlesSize) {
			b = true;
			for (int i = currentParticlesSize; i<size; ++i) {
				QeVertex particle = createParticleData();
				if (!bFollowPos) {
					particle.pos.x += parentModel._30;
					particle.pos.y += parentModel._31;
					particle.pos.z += parentModel._32;
				}
				particles.push_back(particle);
			}
			currentParticlesSize = size;
		}
	}

	if (b) {
		currentParticlesSize = int(particles.size());
		VK->setMemoryBuffer(vertexBuffer, sizeof(particles[0])*particles.size(), particles.data());
		//memset(bDeaths.data(), 0, sizeof(bDeaths[0])*bDeaths.size());
		//VK->setMemoryBuffer(outBuffer, sizeof(bDeaths[0])*bDeaths.size(), bDeaths.data());
		VP->bUpdateDrawCommandBuffers = true;
	}
	updateBuffer();
}