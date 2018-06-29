#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.vert"

void main() 
{
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	vec4 totalTangent = vec4(0.0);

	for(int i=0;i<4;++i){

		mat4 jointTransform = modelData.joints[int(inJoint[i])];
		vec4 posePosition = jointTransform * inPosition;
		totalLocalPos += posePosition * inWeight[i];
		
		vec4 worldNormal = jointTransform * inNormal;
		totalNormal += worldNormal * inWeight[i];

		vec4 worldTagnet = jointTransform * inTangent;
		totalTangent += worldTagnet * inWeight[i];
	}
	gl_Position = totalLocalPos;

	outNormal = totalNormal.xyz;
	outTangent = totalTangent;
	outColor = inColor.xyz;
	outUV = inUV.xy;
}