#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	//outColor = vec4(inColor, 1);
	outColor = adjustColor(inColor, 1);
}