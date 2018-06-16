#version 450
#extension GL_ARB_viewport_array : enable

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) out vec2 outUV;

void main(void) {

	gl_Position = vec4( -1.0, -1.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 0.0 );
	EmitVertex();

	gl_Position = vec4( -1.0, 3.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 2.0 );
	EmitVertex();
  
	gl_Position = vec4( 3.0, -1.0, 0.0, 1.0 );
	outUV = vec2( 2.0, 0.0 );
	EmitVertex();
		
	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;

	EndPrimitive();
}