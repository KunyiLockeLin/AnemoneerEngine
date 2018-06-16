#version 450
#extension GL_ARB_viewport_array : enable

const int MAX_JOINT_NUM = 20;

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

struct QeDataCamera {
	vec4 pos;
	mat4 view;
	mat4 projection;
};

layout( set = 0, binding = 0) uniform QeDataEnvironment {
	vec4 ambintColor;
	QeDataCamera camera;
	vec4 param; // 0: light num
} environmentData;

struct QeDataMaterial {
	vec4 baseColor;
	vec4 metallicRoughness;
	vec4 emissive;
};

layout( set = 1, binding = 0) uniform QeDataModel {
	mat4 model;
	mat4 normal;
	mat4 joints[MAX_JOINT_NUM];
	vec4 param; // 0: billboardType / bCubemap, 1: particleFollow(2:follow 1:unfollow 0:none)
	QeDataMaterial material;
} modelData;

layout(location = 0) in vec3 inColor[];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main(void)
{	
	outColor = inColor[0];

	vec3 size = vec3(modelData.model[0].x,modelData.model[1].y,modelData.model[2].z);
	if( modelData.param.x == 1 ){
		float len = distance(modelData.model[3], environmentData.camera.pos)/10;
		size *= len;
	}	
	if(size.x >0 && size.y >0 && size.z >0 ){ 
		mat4 model = modelData.model;
		if(modelData.param.y == 1) model[3].xyz = gl_in[0].gl_Position.xyz;
		else					   model[3].xyz += gl_in[0].gl_Position.xyz;

		mat4 viewModel = environmentData.camera.view * model;
		viewModel[0].x = size.x;
		viewModel[0].y = 0;
		viewModel[0].z = 0;
	
		viewModel[1].x = 0;
		viewModel[1].y = size.y;
		viewModel[1].z = 0;

		viewModel[2].x = 0;
		viewModel[2].y = 0;
		viewModel[2].z = size.z;

		mat4 mvp = environmentData.camera.projection*viewModel;

		gl_Position = mvp*vec4( 0.0, 2.0, 0.0, 1.0 );
		outTexCoord = vec2( -1.0, 0.0 );
		EmitVertex();

		gl_Position = mvp*vec4( 0.0, 0.0, 0.0, 1.0 );
		outTexCoord = vec2( 1.0, 0.0 );
		EmitVertex();
  
  		gl_Position = mvp*vec4( 2.0, 0.0, 0.0, 1.0 );
		outTexCoord = vec2( 1.0, 2.0 );
		EmitVertex();
	}
	gl_ViewportIndex = gl_InvocationID;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	
	EndPrimitive();
}