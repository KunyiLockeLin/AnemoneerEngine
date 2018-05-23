#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout( triangles ) in;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec4 inTangent[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outTangent;

void main() {
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);
	
	outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];
	outTexCoord = gl_TessCoord.x * inTexCoord[0] + gl_TessCoord.y * inTexCoord[1] + gl_TessCoord.z * inTexCoord[2];
	outNormal = gl_TessCoord.x * inNormal[0] + gl_TessCoord.y * inNormal[1] + gl_TessCoord.z * inNormal[2];
	outTangent = gl_TessCoord.x * inTangent[0] + gl_TessCoord.y * inTangent[1] + gl_TessCoord.z * inTangent[2];
}
