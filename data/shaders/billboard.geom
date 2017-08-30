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
	vec4 param; // 1: viewportNum, 2:billboardType
} ubo;


layout(location = 0) in vec3 inColor[];
layout(location = 1) in vec2 inTexCoord[];
layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;


void main(void)
{	
	if( ubo.param.x <= gl_InvocationID) return;

	mat4 viewModel = ubo.view[gl_InvocationID] * ubo.model;
	vec3 size = vec3(ubo.model[0].x,ubo.model[1].y,ubo.model[2].z);

	if( ubo.param.y == 1 ){
		float len = distance(ubo.model[3], ubo.cameraPos[gl_InvocationID])/10;
		size *= len;
	}

	for(int i = 0; i < gl_in.length(); i++){

		outColor = inColor[i];
		outTexCoord = inTexCoord[i];

		viewModel[0].x = size.x;
		viewModel[0].y = 0;
		viewModel[0].z = 0;
		
		viewModel[1].x = 0;
		viewModel[1].y = size.y;
		viewModel[1].z = 0;

		viewModel[2].x = 0;
		viewModel[2].y = 0;
		viewModel[2].z = size.z;
		
		vec4 posC = viewModel * gl_in[i].gl_Position;
		
		gl_Position = ubo.proj[gl_InvocationID] * posC;
		
		// Set the viewport index that the vertex will be emitted to
		gl_ViewportIndex = gl_InvocationID;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}