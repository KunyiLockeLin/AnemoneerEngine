#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( binding = 1) uniform sampler2D texSampler;

layout( binding = 2) uniform QeDataLight {
    vec4 pos;
    vec4 dir;
	vec4 color;
	int type;
	float intensity;
	float radius;
} light;

layout( binding = 3) uniform QeDataMaterial {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	vec4 emissive;
	float refraction;
	float specularExponent;
	float alpha;
} mtl;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragLighttoVertex;

layout(location = 0) out vec4 outColor;


void main() {
	
	float fDist = length(fragLighttoVertex); 
    vec3 vLighttoVertex = normalize(fragLighttoVertex);
	float lightIntensity = clamp(dot(fragNormal,vLighttoVertex), 0, 1);//*max(1.0-fDist/light.intensity,0.0 );
	//outColor = texture(texSampler, fragTexCoord)*light.color*lightIntensity;
	outColor =vec4(fragNormal,1);
}