#include "header.glsl"

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(triangles, equal_spacing, cw) in;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inUV[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec3 inTangent[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec3 outBiTanget;
layout(location = 5) out vec3 outPostion;
layout(location = 6) out vec3 outLighttoVertex;
layout(location = 7) out vec3 outUV3;
