#version 450
#extension GL_ARB_viewport_array : enable

const int MAX_VIEWPORT_NUM = 9;

layout (triangles, invocations = MAX_VIEWPORT_NUM) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec2 inUV[];
layout (location = 0) out vec2 outUV;


void main(void) {

	for(int i = 0; i < gl_in.length(); i++){

		outUV = inUV[i];
		gl_Position = gl_in[i].gl_Position;
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}