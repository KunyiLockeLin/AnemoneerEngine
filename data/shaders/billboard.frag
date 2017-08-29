#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( binding = 1) uniform QeDataLight {
    vec4 pos;
    vec4 dir;
	vec4 color;
	int type; //	0: eLightPoint, 1: eLightDirection, 2: eLightSpot,
	float intensity;
	float coneAngle;
} light;

layout( binding = 2) uniform QeDataMaterial {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	vec4 emissive;
	float refraction;
	float specularExponent;
	float alpha;
} mtl;

layout( binding = 3) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inLighttoVertex;
layout(location = 4) in vec3 inEye;

layout(location = 0) out vec4 outColor;


void main() {
	
	vec4 texColor = texture(texSampler, inTexCoord);
	outColor =	texColor*mtl.diffuse*texColor.a;
}