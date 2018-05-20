#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 4) uniform samplerCube cubeSampler;

layout(location = 0) in vec3 inTexCoord;
layout(location = 0) out vec4 outColor;


void main() {
	//outColor = vec4(inTexCoord, 1.0f);
	//outColor = texture(cubeSampler, vec3(0,0,1));
	outColor = texture(cubeSampler, inTexCoord);
}