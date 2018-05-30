#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property) {
	
	initProperty = _property;
	
	const char * c = AST->getXMLValue(_property, 1, "id");
	if (c != nullptr)	id = atoi(c);

	particleRule = AST->getParticle(c);

	// count
	particlesSize = MATH->iRandom(particleRule->count_life.x, particleRule->count_life.y);
	particles.resize(particlesSize);

	for (uint32_t i = 0; i < particlesSize; ++i) {

		// life time
		particles[i].normal.w = MATH->iRandom(particleRule->count_life.z, particleRule->count_life.w);

		// pos
		particles[i].pos.x = MATH->fRandom(particleRule->init_pos_xy.x, particleRule->init_pos_xy.y);
		particles[i].pos.y = MATH->fRandom(particleRule->init_pos_xy.z, particleRule->init_pos_xy.w);
		particles[i].pos.z = MATH->fRandom(particleRule->init_pos_zr.x, particleRule->init_pos_zr.y);
		if (particleRule->init_pos_zr.z != 0 || particleRule->init_pos_zr.w != 0) {
			float radius = MATH->fRandom(particleRule->init_pos_zr.z, particleRule->init_pos_zr.w);
			float radian = MATH->fRandom(particleRule->init_pos_d.x, particleRule->init_pos_d.y) * MATH->DEGREES_TO_RADIANS;
			particles[i].pos.x = radius * cos(radian);
			particles[i].pos.y = radius * sin(radian);
		}

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

		// force

		/*
		QeVector4f init_speed_z_force_x;// 0: speed_z, 3: speed_z_range, 2: force_x 1: force_x_range
		QeVector4f force_yz;			// 0: force_y 1: force_y_range, 2:force_z 3: force_z_range
		QeVector4f rotate_xy;			// 0: rotate_x(degree), 1: roate_x_range, 2: rotate_y, 3: rotate_y_range,
		QeVector4f rotate_z_color_r;	// 0: rotate_z, 1: roate_y_range, 2: color_r 3: color_r_range
		QeVector4f color_gb;			// 0: color_g, 1: color_g_range, 2: color_b 3: color_b_range
		QeVector4f alpha_size_x;		// 0: alpha, 1: ?, 2: size_x, 3: size_x_range
		QeVector4f size_yz;				// 0: size_y, 1: size_y_range, 2:size_z 3: size_z_range
		QeVector4f init_pos_d;			// 0: degree, 1: degree_range
		*/
		MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].pos.x);
		MATH->fRandoms(0.0f, 1.0f, 3, &particles[i].color.x);
		MATH->fRandoms(-1.0f, 2.0f, 3, &particles[i].normal.x);
	}

	VK->createBufferData((void*)particles.data(), sizeof(particles[0]) * particles.size(), VertexBuffer.buffer, VertexBuffer.memory);
	VK->createBufferView(VertexBuffer.buffer, VK_FORMAT_R32G32B32A32_SFLOAT, VertexBuffer.view);

	//modelData = AST->getModel("point");
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;
	data.uboBuffer = uboBuffer.buffer;
	data.inputStorageTexeLBufferView = VertexBuffer.view;
	data.outputStorageTexeLBufferView = VertexBuffer.view;

	VK->updateDescriptorSet(data, descriptorSet);

	pos = { 0, 0, 0 };
	face = 0.0f;
	up = 0.0f;
	size = { 1,1,1 };
	speed = 0;
	currentActionID = 0;
	actionType = eActionTypeOnce;
	actionState = eActionStateStop;
	attachID = 0;
	attachSkeletonName = nullptr;
	actionSpeed = 0;

	AST->setShader(shader, _property, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "particle"));
	computeShader = AST->getShader(AST->getXMLValue(4, AST->CONFIG, "defaultShader", "particle", "comp"));

	createPipeline();
}

void QeParticle::createPipeline() {

	graphicsPipeline = VK->createGraphicsPipeline(&shader, ePipeLine_Point );
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
	VP->bUpdateDrawCommandBuffers = true;
}

void QeParticle::setMatModel() {

	//type = eBillboardFaceAndSize;

	QeMatrix4x4f mat;
	mat *= MATH->translate(pos);
	//size = { 3,3,3 };
	mat *= MATH->scale(size);

	ubo.model = mat;
}