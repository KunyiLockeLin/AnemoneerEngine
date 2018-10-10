#version 450
#extension GL_GOOGLE_include_directive : enable
#include "headerIn.vert"
#include "headerOut.vert"


void main() 
{	
	outPostion = (modelData.model *inPosition).xyz;	
	gl_Position = environmentData.camera.projection * environmentData.camera.view *vec4(outPostion, 1);
	//outNormal = normalize(vec3(modelData.model * inNormal));
	//outNormal = normalize(vec3(modelData.model * vec4(inNormal.xyz, 0.0)));
	outNormal = normalize(mat3(modelData.model) * inNormal.xyz);
	outTangent = normalize(vec3(modelData.model * inTangent));
	//outTangent = normalize(mat3(modelData.model) * inTangent.xyz);
	outBiTanget = normalize(cross(outNormal, outTangent));	
	outColor = inColor.xyz;
	outUV = inUV.xy;
}