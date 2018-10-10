#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main(void)
{	
	mat4 outline=mat4(modelData.param2[0]+1);
	outline[3].w = 1;

	mat4 mvp = environmentData.camera.projection * environmentData.camera.view * modelData.model*outline;

	for(int i = 0; i < gl_in.length(); i++) {
		
		outColor = inColor[i];
		outUV = inUV[i];
		outUV3 = gl_in[i].gl_Position.xyz;
		//vec4 pos = vec4(gl_in[i].gl_Position.xyz + inNormal[i] * modelData.param[2], gl_in[i].gl_Position.w);
		//gl_Position = mvp * pos;
		gl_Position = mvp * gl_in[i].gl_Position;
	
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}