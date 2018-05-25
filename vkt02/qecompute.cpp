#include "qeheader.h"

QeCompute::~QeCompute() {}


void QeCompute::init() {

	shader = AST->getShader("shaderc.spv");
	pipeline = VK->createComputePipeline(shader);
}