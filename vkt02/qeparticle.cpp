#include "qeheader.h"
/*
void QeParticle::init(QeAssetXML* _property) {

	particles.resize(PARTICLES_COUNT);

	for (uint32_t i = 0; i < PARTICLES_COUNT; ++i) {
		particles[i].pos = { 0.0f,0.0f,0.0f };
		MATH->fRandom(0.0f, 1.0f, 3, &particles[i].color.x);
		MATH->fRandom(0.0f, 10.0f, 2, &particles[i].normal.x);
	}

	VK->createBufferData((void*)particles.data(), sizeof(particles[0]) * particles.size(), VertexBuffer.buffer, VertexBuffer.memory);
	VK->createBufferView(VertexBuffer.buffer, VK_FORMAT_R32G32B32A32_SFLOAT, VertexBuffer.view);

	QeDataDescriptorSet data;
	data.storageTexeLBufferView = VertexBuffer.view;

	VK->updateDescriptorSet(data, descriptorSet);

	shader = AST->getShader("shaderc.spv");
	pipeline = VK->createComputePipeline(shader);

	timer.initTime();
}

void QeParticle::updateComputeCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdDispatch(drawCommandBuffer, PARTICLES_COUNT / 32 + 1, 1, 1);
}

void QeParticle::updateDrawCommandBuffer(VkCommandBuffer& drawCommandBuffer) {

	//if (!bShow || !bCullingShow) return;

	vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdDraw(drawCommandBuffer, PARTICLES_COUNT, 1, 0, 0);
}

void QeParticle::updateCompute(float time) {
	VK->pushConstants[0] = float(timer.getPassTime());
	VP->bUpdateDrawCommandBuffers = true;
}
*/