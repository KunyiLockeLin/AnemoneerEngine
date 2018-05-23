#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout( vertices = 3 ) out;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec4 inTangent[];

layout(location = 0) out vec3 outColor[3];
layout(location = 1) out vec2 outTexCoord[3];
layout(location = 2) out vec3 outNormal[3];
layout(location = 3) out vec4 outTangent[3];

void main(void) {
  if( gl_InvocationID == 0 ) {
    gl_TessLevelInner[0] = 2.0;
    gl_TessLevelOuter[0] = 1.0;
    gl_TessLevelOuter[1] = 2.0;
    gl_TessLevelOuter[2] = 3.0;
  }
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  outColor[gl_InvocationID] = inColor[gl_InvocationID];
  outTexCoord[gl_InvocationID] = inTexCoord[gl_InvocationID];
  outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
  outTangent[gl_InvocationID] = inTangent[gl_InvocationID];
}
