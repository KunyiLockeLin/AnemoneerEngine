#version 450
#extension GL_ARB_viewport_array : enable

const int MAX_VIEWPORT_NUM = 9;

layout (points, invocations = MAX_VIEWPORT_NUM) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) out vec2 outTexCoord;


void main(void) {

	for(int i = 0; i < gl_in.length(); i++) {

		gl_Position = vec4( -1.0, -1.0, 0.0, 1.0 );
		outTexCoord = vec2( 0.0, 0.0 );
		EmitVertex();

		gl_Position = vec4( -1.0, 3.0, 0.0, 1.0 );
		outTexCoord = vec2( 0.0, 2.0 );
		EmitVertex();
  
		gl_Position = vec4( 3.0, -1.0, 0.0, 1.0 );
		outTexCoord = vec2( 2.0, 0.0 );
		EmitVertex();
		
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
	}	
	EndPrimitive();
}