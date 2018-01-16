#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_VIEWPORT_NUM = 9;

layout( binding = 0) uniform QeUniformBufferObject {
	mat4 model;
    mat4 view[MAX_VIEWPORT_NUM];
    mat4 proj[MAX_VIEWPORT_NUM];
	mat4 normal[MAX_VIEWPORT_NUM];
	mat4 joints[3];
	vec4 jointIDs;
	vec4 cameraPos[MAX_VIEWPORT_NUM];
	vec4 ambientColor;
	vec4 param; // 1: viewportNum, 2:billboardType
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec4 inTangent;
layout(location = 5) in ivec4 inJoint;
layout(location = 6) in vec4 inWeight;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;

void main() 
{
	if(ubo.jointIDs.w == 1){
		vec4 totalLocalPos = vec4(0.0);
		vec4 totalNormal = vec4(0.0);
		vec4 totalTangent = vec4(0.0);

		for(int i=0;i<3;++i){

			mat4 jointTransform = ubo.joints[inJoint[i]];
			vec4 posePosition = jointTransform * vec4(inPosition, 1.0);
			totalLocalPos += posePosition * inWeight[i];
		
			vec4 worldNormal = jointTransform * vec4(inNormal, 0.0);
			totalNormal += worldNormal * inWeight[i];

			vec4 worldTagnet = jointTransform * inTangent;
			totalTangent += worldTagnet * inWeight[i];
		}
	
		gl_Position = totalLocalPos;
		outNormal = totalNormal.xyz;
		outTangent = totalTangent;
	}
	else{
		gl_Position = vec4(inPosition.xyz, 1.0);	
		outNormal = inNormal;
		outTangent = inTangent;
	}
	outColor = inColor;
	outTexCoord = inTexCoord;
}