#version 450
#extension GL_ARB_viewport_array : enable
#extension GL_ARB_shading_language_420pack : enable

const int MAX_VIEWPORT_NUM = 9;
const int MAX_JOINT_NUM = 20;

layout (lines, invocations = MAX_VIEWPORT_NUM) in;
layout (line_strip, max_vertices = 2) out;

layout( binding = 0) uniform QeUniformBufferObject {
	mat4 model;
    mat4 view[MAX_VIEWPORT_NUM];
    mat4 proj[MAX_VIEWPORT_NUM];
	mat4 normal[MAX_VIEWPORT_NUM];
	mat4 joints[MAX_JOINT_NUM];
	vec4 cameraPos[MAX_VIEWPORT_NUM];
	vec4 ambientColor;
	vec4 param; // 1: viewportNum, 2:billboardType
} ubo;

layout(location = 0) in vec3 inColor[];
layout(location = 2) in vec3 inNormal[]; // line direction and length

layout(location = 0) out vec3 outColor;

void main(void)
{	
	if( ubo.param.x <= gl_InvocationID) return;

	for(int i = 0; i < gl_in.length(); i++) {
		
		outColor = inColor[i];
		gl_Position = ubo.proj[gl_InvocationID] *ubo.view[gl_InvocationID] * ubo.model * gl_in[i].gl_Position;
		EmitVertex();
		
		gl_Position = ubo.proj[gl_InvocationID] *ubo.view[gl_InvocationID] * ubo.model * (gl_in[i].gl_Position + vec4( inNormal[i], 0.0 ));
		EmitVertex();
		
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
	}
	EndPrimitive();
}