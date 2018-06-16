#include "header.glsl"
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout( set = 1, binding = 10) uniform sampler2D	baseColorMapSampler;
layout( set = 1, binding = 11) uniform samplerCube	cubeMapSampler;
layout( set = 1, binding = 12) uniform sampler2D	normalMapSampler;

layout (input_attachment_index=0, set=2, binding=0) uniform subpassInput inputAttachment;


layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTanget;
layout(location = 5) in vec3 inPostion;
layout(location = 6) in vec3 inLighttoVertex;
layout(location = 7) in vec3 inUV3;

layout(location = 0) out vec4 outColor;