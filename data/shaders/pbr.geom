#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"


layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


void main(void) {
	
	mat4 vp = environmentData.camera.projection * environmentData.camera.view;
	for(int i = 0; i < gl_in.length(); i++){

		outColor = inColor[i];
		outUV = inUV[i];
		//outNormal = normalize(vec3(modelData.model * vec4(inNormal[i], 0.0)));
		outNormal = normalize(mat3(modelData.model) * inNormal[i]);
		//outNormal = inNormal[i];
		outTangent = normalize(vec3(modelData.model * vec4(inTangent[i].xyz, 0.0)));
		//outTangent = inTangent[i].xyz;
		outBiTanget = normalize(cross(outNormal, outTangent));	
		outPostion = (modelData.model*gl_in[i].gl_Position).xyz;
		//outPostion = gl_in[i].gl_Position.xyz;
		outLighttoVertex = vec3(lights[0].pos) - outPostion;

		gl_Position = vp * vec4(outPostion,1);

		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}