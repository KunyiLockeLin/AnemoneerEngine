#version 450
#extension GL_ARB_viewport_array : enable

const int MAX_VIEWPORT_NUM = 9;

layout (triangles, invocations = MAX_VIEWPORT_NUM) in;
layout (triangle_strip, max_vertices = 3) out;

layout( binding = 0) uniform QeUniformBufferObject {
	mat4 model;
    mat4 view[MAX_VIEWPORT_NUM];
    mat4 proj[MAX_VIEWPORT_NUM];
	mat4 normal[MAX_VIEWPORT_NUM];
	vec4 cameraPos[MAX_VIEWPORT_NUM];
	vec4 ambientColor;
	vec4 param; // 1: viewportNum, 2:billboardType
} ubo;

layout( binding = 1) uniform QeDataLight {
	vec4 pos;
    vec4 dir;
	vec4 color;
	vec4 param; // 1: type, 2: intensity, 3: coneAngle
} light;

layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];
layout(location = 3) in vec4 inTangent[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec3 outBiTanget;
layout(location = 5) out vec3 outPostion;
layout(location = 6) out vec3 outCameraPostion;
layout(location = 7) out vec3 outLighttoVertex;


void main(void) {

	if( ubo.param.x <= gl_InvocationID) return;

	for(int i = 0; i < gl_in.length(); i++){

		outPostion = (ubo.model*gl_in[i].gl_Position).xyz;
		outCameraPostion = vec3(ubo.cameraPos[gl_InvocationID]);
		outColor = inColor[i];
		outTexCoord = inTexCoord[i];
		outNormal = normalize(vec3(ubo.model * vec4(inNormal[i], 0.0)));
		outTangent = normalize(vec3(ubo.model * vec4(inTangent[i].xyz, 0.0)));
		outBiTanget = cross(outNormal, outTangent) * inTangent[i].w;	
		outLighttoVertex = vec3(light.pos) - outPostion;

		gl_Position = ubo.proj[gl_InvocationID] * ubo.view[gl_InvocationID] * vec4(outPostion,1);
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}