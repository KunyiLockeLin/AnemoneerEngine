#include "header.glsl"
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout( set = 1, binding = 10) uniform sampler2D baseColorMap;
layout( set = 1, binding = 11) uniform samplerCube cubeSampler;
layout( set = 1, binding = 12) uniform sampler2D normalSampler;

layout (input_attachment_index=0, set=2, binding=0) uniform subpassInput inputAttachment;


layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;