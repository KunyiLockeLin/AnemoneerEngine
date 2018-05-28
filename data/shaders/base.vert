#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec4 inTexCoord;
layout(location = 3) in vec4 inNormal;
layout(location = 4) in vec4 inTangent;
layout(location = 5) in vec4 inJoint;
layout(location = 6) in vec4 inWeight;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;

void main() 
{
	gl_Position = inPosition;	
	outNormal = inNormal.xyz;
	outTangent = inTangent;
	outColor = inColor.xyz;
	outTexCoord = inTexCoord.xy;
}