#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (triangles, equal_spacing, cw ) in;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec4 inTangent[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;

void main(void) {
	gl_Position = (gl_TessCoord[0] * gl_in[0].gl_Position) +
                  (gl_TessCoord[1] * gl_in[1].gl_Position) +
                  (gl_TessCoord[2] * gl_in[2].gl_Position);
	
	outColor = gl_TessCoord[0] * inColor[0] + gl_TessCoord[1] * inColor[1] + gl_TessCoord[2] * inColor[2];
	outTexCoord = gl_TessCoord[0] * inTexCoord[0] + gl_TessCoord[1] * inTexCoord[1] + gl_TessCoord[2] * inTexCoord[2];
	outNormal = gl_TessCoord[0] * inNormal[0] + gl_TessCoord[1] * inNormal[1] + gl_TessCoord[2] * inNormal[2];
	outTangent = gl_TessCoord[0] * inTangent[0] + gl_TessCoord[1] * inTangent[1] + gl_TessCoord[2] * inTangent[2];
}
