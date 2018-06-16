#extension GL_ARB_separate_shader_objects : enable

const int MAX_JOINT_NUM = 20;

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

layout( set = 1, binding = 10) uniform sampler2D baseColorMap;