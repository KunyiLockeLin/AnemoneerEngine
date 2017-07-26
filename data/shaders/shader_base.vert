#version 450
#extension GL_ARB_separate_shader_objects : enable


layout( binding = 0) uniform QeDataMVP {
	mat4 model;
    mat4 view;
    mat4 proj;
	mat4 normal;
} mvp;

layout( binding = 2) uniform QeDataLight {
	vec4 pos;
    vec4 dir;
	vec4 color;
	int type;
	float intensity;
	float radius;
} light;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragLighttoVertex;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	
	vec4 pos = mvp.view * mvp.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
	//fragNormal = normalize( mvp.normal * inNormal);
	mat3 normalMatrix = mat3(transpose(inverse(mvp.view * mvp.model)));
	fragNormal = normalize( normalMatrix *inNormal);
	fragLighttoVertex = vec3(light.pos - pos);
	gl_Position = mvp.proj * pos;
}