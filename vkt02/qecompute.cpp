#include "qeheader.h"

QeCompute::~QeCompute() {}


void QeCompute::init() {

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
}