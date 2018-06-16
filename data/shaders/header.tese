#include "header.glsl"

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (triangles, equal_spacing, cw ) in;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inUV[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec4 inTangent[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;