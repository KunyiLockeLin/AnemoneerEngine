#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.tesc"

void main(void) {
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 2.0;
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 2.0;
        gl_TessLevelOuter[2] = 3.0;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    outColor[gl_InvocationID] = inColor[gl_InvocationID];
    outUV[gl_InvocationID] = inUV[gl_InvocationID];
    outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
    outTangent[gl_InvocationID] = inTangent[gl_InvocationID];
}
