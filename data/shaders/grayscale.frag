#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	//outColor = subpassLoad(inputAttachment);
	outColor = texture(inputAttachment, inUV);
 	outColor.r = (outColor.r+outColor.g +outColor.b)/3; 
	outColor.g = outColor.r;
	outColor.b = outColor.r; 
}