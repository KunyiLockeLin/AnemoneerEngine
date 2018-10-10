#version 450
#extension GL_GOOGLE_include_directive : enable
#include "headerIn.vert"
#include "headerOut.vert"


void main() 
{
	gl_Position = inPosition;	
	//gl_Position = modelData.model *inPosition;	
	//outNormal2 = inNormal.xyz;
	outNormal = inNormal.xyz;
	//outNormal = normalize(vec3(modelData.model * inNormal));
	outTangent = inTangent.xyz;
	//outTangent = normalize(modelData.model * inTangent);
	outColor = inColor.xyz;
	outUV = inUV.xy;
}