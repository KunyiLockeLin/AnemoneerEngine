#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.tese"

void main(void) {
    gl_Position = (gl_TessCoord[0] * gl_in[0].gl_Position) + (gl_TessCoord[1] * gl_in[1].gl_Position) +
                  (gl_TessCoord[2] * gl_in[2].gl_Position);

    outColor = gl_TessCoord[0] * inColor[0] + gl_TessCoord[1] * inColor[1] + gl_TessCoord[2] * inColor[2];
    outUV = gl_TessCoord[0] * inUV[0] + gl_TessCoord[1] * inUV[1] + gl_TessCoord[2] * inUV[2];
    outNormal = gl_TessCoord[0] * inNormal[0] + gl_TessCoord[1] * inNormal[1] + gl_TessCoord[2] * inNormal[2];
    outTangent = gl_TessCoord[0] * inTangent[0] + gl_TessCoord[1] * inTangent[1] + gl_TessCoord[2] * inTangent[2];
}
