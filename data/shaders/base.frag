#version 450
#extension GL_GOOGLE_include_directive : enable
#include "headerFrag.glsl"


void main() {
	outColor = vec4(inColor, 1);
}