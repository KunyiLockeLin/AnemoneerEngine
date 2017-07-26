#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 2) uniform QeDataLight {
    vec3 pos;
    vec3 dir;
    vec3 color;
	int type;
	float intensity;
	float radius;
} light;

layout(set = 0, binding = 3) uniform QeDataMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 emissive;
	float refraction;
	float specularExponent;
	float alpha;
} mtl;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}