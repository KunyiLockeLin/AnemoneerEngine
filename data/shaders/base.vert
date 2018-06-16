#version 450
#extension GL_GOOGLE_include_directive : enable
#include "headerVert.glsl"


void main() 
{
	gl_Position = inPosition;	
	outNormal = inNormal.xyz;
	outTangent = inTangent;
	outColor = inColor.xyz;
	outUV = inUV.xy;
}