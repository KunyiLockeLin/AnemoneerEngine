#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


void main(void) {
	
	mat4 mvp = environmentData.camera.projection * environmentData.camera.view * modelData.model;
	for(int i = 0; i < gl_in.length(); i++){

		outCameraPostion = vec3(environmentData.camera.pos);
		outColor = inColor[i];
		outUV = inUV[i];
		outNormal = normalize(vec3(modelData.model * vec4(inNormal[i], 0.0)));
		outTangent = normalize(vec3(modelData.model * vec4(inTangent[i].xyz, 0.0)));
		outBiTanget = cross(outNormal, outTangent) * inTangent[i].w;	
		//outPostion = (modelData.model*gl_in[i].gl_Position).xyz;
		//outLighttoVertex = vec3(lights[0].pos) - outPostion;
		outLighttoVertex = vec3(lights[0].pos) - (modelData.model*gl_in[i].gl_Position).xyz;

		//gl_Position = environmentData.camera.projection * environmentData.camera.view * vec4(outPostion,1);
		gl_Position = mvp * gl_in[i].gl_Position;

		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}