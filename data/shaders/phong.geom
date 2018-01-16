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
	mat4 joints[3];
	vec4 jointIDs;
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

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outLighttoVertex;
layout(location = 4) out vec3 outEye;


void main(void)
{	
	if( ubo.param.x <= gl_InvocationID) return;

	for(int i = 0; i < gl_in.length(); i++)
	{
		outColor = inColor[i];
		outTexCoord = inTexCoord[i];
		
		//outNormal = normalize( vec3(ubo.view[gl_InvocationID] * ubo.model * vec4(inNormal[i],0)));
		outNormal = normalize( vec3(ubo.normal[gl_InvocationID] * vec4(inNormal[i],0)));
		//mat4 normalMatrix = transpose(inverse(ubo.view[gl_InvocationID] * ubo.model));
		//outNormal = normalize( vec3(normalMatrix * vec4(inNormal[i],0)));
		
		vec4 posC = ubo.view[gl_InvocationID] * ubo.model * gl_in[i].gl_Position;
		outEye = vec3(-posC);
		outLighttoVertex = vec3( ubo.view[gl_InvocationID] * light.pos - posC);
		
		gl_Position = ubo.proj[gl_InvocationID] * posC;
		//gl_Position = ubo.proj[gl_InvocationID] *ubo.view[gl_InvocationID] * ubo.model * gl_in[i].gl_Position;
	
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}