#version 450
#extension GL_GOOGLE_include_directive : enable

#include "header.glsl"

layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;


void main() {

	vec4 texColor = texture(baseColorMap, inUV);
	if( 0.01 > texColor.a ) discard;

	outColor = texColor*modelData.mtl.baseColor*texColor.a;
}