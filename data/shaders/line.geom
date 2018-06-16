#version 450
#extension GL_ARB_viewport_array : enable
#extension GL_ARB_shading_language_420pack : enable

const int MAX_JOINT_NUM = 20;

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

struct QeDataCamera {
	vec4 pos;
	mat4 view;
	mat4 projection;
};

layout( set = 0, binding = 0) uniform QeDataEnvironment {
	vec4 ambintColor;
	QeDataCamera camera;
	vec4 param; // 0: light num
} environmentData;

layout( set = 0, binding = 10) buffer QeDataLight {
	vec4 pos;
	vec4 dir;
	vec4 color;
	vec4 param; // 1: type, 2: intensity, 3: coneAngle
} light;

struct QeDataMaterial {
	vec4 baseColor;
	vec4 metallicRoughness;
	vec4 emissive;
};

layout( set = 1, binding = 0) uniform QeDataModel {
	mat4 model;
	mat4 normal;
	mat4 joints[MAX_JOINT_NUM];
	vec4 param; // 0: billboardType / bCubemap, 1: particleFollow(2:follow 1:unfollow 0:none)
	QeDataMaterial mtl;
} modelData;

layout(location = 0) in vec3 inColor[];
layout(location = 2) in vec3 inNormal[]; // line direction and length

layout(location = 0) out vec3 outColor;

void main(void)
{	
	outColor = inColor[0];
	mat4 mvp = environmentData.camera.projection * environmentData.camera.view * modelData.model;
		
	gl_Position = mvp * gl_in[0].gl_Position;
	EmitVertex();
		
	gl_Position = mvp * (gl_in[0].gl_Position + vec4( inNormal[0], 0.0 ));
	EmitVertex();

	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;

	EndPrimitive();
}