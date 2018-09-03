#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	outColor = subpassLoad(inputAttachment);
	if( outColor.r>1 || outColor.g>1 || outColor.b>1 ) outColor = vec4(1,0,0,1);
}