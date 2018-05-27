#include "qeheader.h"

void QeParticle::init(QeAssetXML* _property) {
	
	initProperty = _property;

	particles.resize(PARTICLES_COUNT);

	for (uint32_t i = 0; i < PARTICLES_COUNT; ++i) {
		particles[i].pos = { 0.0f,0.0f,0.0f };
		MATH->fRandom(-10.0f, 10.0f, 3, &particles[i].pos.x);
		MATH->fRandom(0.0f, 1.0f, 3, &particles[i].color.x);
		MATH->fRandom(-10.0f, 10.0f, 3, &particles[i].normal.x);
	}

	VK->createBufferData((void*)particles.data(), sizeof(particles[0]) * particles.size(), VertexBuffer.buffer, VertexBuffer.memory);
	VK->createBufferView(VertexBuffer.buffer, VK_FORMAT_R32G32B32A32_SFLOAT, VertexBuffer.view);

	//modelData = AST->getModel("point");
	descriptorSet = VK->createDescriptorSet(VK->descriptorSetLayout);

	VK->createUniformBuffer(sizeof(QeUniformBufferObject), uboBuffer.buffer, uboBuffer.memory);

	QeDataDescriptorSet data;
	data.uboBuffer = uboBuffer.buffer;
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
	timer.initTime();

	AST->setShader(shader, _property, AST->getXMLNode(3, AST->CONFIG, "defaultShader", "particle"));
	computeShader = AST->getShader("shaderc.spv");

	createPipeline();
}

void QeParticle::createPipeline() {

	graphicsPipeline = VK->createGraphicsPipeline(&shader, ePipeLine_Point );
	computePipeline = VK->createComputePipeline(computeShader);
}

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	//vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	//vkCmdDispatch(drawCommandBuffer, PARTICLES_COUNT / 32 + 1, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//if (!bShow || !bCullingShow) return;
	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &VertexBuffer.buffer, offsets);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdDraw(drawCommandBuffer, PARTICLES_COUNT, 1, 0, 0);
}

void QeParticle::updateCompute(float time) {
	VK->pushConstants[0] = float(timer.getPassTime());
	VP->bUpdateDrawCommandBuffers = true;
}