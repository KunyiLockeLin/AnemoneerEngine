#version 450
#extension GL_ARB_viewport_array : enable

const int MAX_VIEWPORT_NUM = 9;
const int MAX_JOINT_NUM = 20;

layout (triangles, invocations = MAX_VIEWPORT_NUM) in;
layout (triangle_strip, max_vertices = 3) out;

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

layout(location = 0) out vec3 outTexCoord;

void main(void)
{	
	if( ubo.param.x <= gl_InvocationID) return;

	for(int i = 0; i < gl_in.length(); i++) {
		outTexCoord = gl_in[i].gl_Position.xyz;
		//outTexCoord.y *= -1;
		gl_Position = ubo.proj[gl_InvocationID] *ubo.view[gl_InvocationID] * ubo.model * gl_in[i].gl_Position;
	
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}