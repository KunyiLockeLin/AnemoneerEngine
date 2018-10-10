#include "header.glsl"
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;
layout(location = 4) out vec3 outNormal2;
layout(location = 5) out vec3 outPostion;
layout(location = 6) out vec3 outLighttoVertex;
layout(location = 7) out vec3 outUV3;