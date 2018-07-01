#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {

	vec4 UV4 = inUV4;
	//vec4 UV4 = inUV4/ inUV4.w;
	vec4 texColor = texture(shadowMapSampler, inUV);
	//vec4 texColor = texture(shadowMapSampler, UV4.xy);
	if( 0.01 > texColor.a ) discard;

	outColor = texColor;
}