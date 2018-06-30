#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {

	vec4 inUV4_2 = inUV4/ inUV4.w;
	vec4 texColor = texture(baseColorMapSampler, inUV);
	outColor = texColor;
}