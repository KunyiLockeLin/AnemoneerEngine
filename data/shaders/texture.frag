#version 450
#extension GL_ARB_separate_shader_objects : enable


layout( binding = 2) uniform QeDataMaterial {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
	vec4 emissive;
	vec4 param; // 1: specularExponent, 2: refraction, 3: alpha,
} mtl;

layout( binding = 3) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;


void main() {
	
	vec4 texColor = texture(texSampler, inTexCoord);
	if( 0.01 > texColor.a ) {
		discard;
	}
	outColor = texColor*mtl.diffuse*texColor.a;
}