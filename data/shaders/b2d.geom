#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;


void main(void)
{	  
	outColor = inColor[0];

	mat4 mvp = modelData.model; 
	mvp[3].xy += environmentData.camera.pos.xy;

	float scale = 1.0/environmentData.camera.pos.z; 
	mvp[0].x *= scale;
	mvp[0].y = 0;
	mvp[0].z = 0;
	mvp[1].x = 0;
	mvp[1].y *= (scale * environmentData.camera.pos.w);
	mvp[1].x = 0;
	mvp[2].x = 0;
	mvp[2].y = 0;
	mvp[2].z *= scale;

	gl_Position = mvp*vec4( 0.0, 0.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 0.0 );
	EmitVertex();

	gl_Position = mvp*vec4( 0.0, 2.0, 0.0, 1.0 );
	outUV = vec2( 0.0, 2.0 );
	EmitVertex();
 
	gl_Position = mvp*vec4( 2.0, 0.0, 0.0, 1.0 );
	outUV = vec2( 2.0, 0.0 );
	EmitVertex();

	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	
	EndPrimitive();
}