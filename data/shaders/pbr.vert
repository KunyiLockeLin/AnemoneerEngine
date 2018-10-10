#version 450
#extension GL_GOOGLE_include_directive : enable
#include "headerIn.vert"
#include "headerOut.vert"


void main() 
{	
	outPostion = (modelData.model *inPosition).xyz;	
	gl_Position = environmentData.camera.projection * environmentData.camera.view *vec4(outPostion, 1);	
	//outNormal = normalize(vec3(modelData.model * inNormal));
	outNormal = normalize(mat3(modelData.model) * inNormal.xyz);
	outTangent = normalize(modelData.model * inTangent);
	outColor = inColor.xyz;
	outUV = inUV.xy;
}