#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout (points) in;
layout (line_strip, max_vertices = 2) out;

//inNormal2[] : line direction and length

void main(void)
{	
	outColor = inColor[0];
	mat4 mvp = environmentData.camera.projection * environmentData.camera.view * modelData.model;
		
	gl_Position = mvp * gl_in[0].gl_Position;
	EmitVertex();
		
	gl_Position = mvp * (gl_in[0].gl_Position + vec4( inNormal2[0], 0.0 ));
	EmitVertex();

	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;

	EndPrimitive();
}