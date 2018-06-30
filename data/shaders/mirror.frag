#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {

	vec4 texColor = texture(baseColorMapSampler, inUV);
	outColor = texColor;
}