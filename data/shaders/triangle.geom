#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;


void main(void)
{	
	outColor = inColor[0];

	mat4 mvp = environmentData.camera.projection * environmentData.camera.view * modelData.model;

	gl_Position = mvp*vec4( 0.0, 0.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 0.0 );
	EmitVertex();

	gl_Position = mvp*vec4( 0.0, -2.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 2.0 );
	EmitVertex();

	gl_Position = mvp*vec4( 2.0, 0.0, 0.0, 1.0 );
	outUV = vec2( 2.0, 0.0 );
	EmitVertex();
	
	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	
	EndPrimitive();
}