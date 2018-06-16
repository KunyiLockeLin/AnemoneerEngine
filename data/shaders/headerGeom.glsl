#include "header.glsl"
#extension GL_ARB_viewport_array : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 inColor[];
layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;
