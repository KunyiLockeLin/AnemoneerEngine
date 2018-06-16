#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	outColor = texture(cubeMapSampler, inUV3);
}