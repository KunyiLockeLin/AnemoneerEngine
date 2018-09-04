#include "header.glsl"
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout( set = layoutSet_Model, binding = Model_image + 0) uniform sampler2D	baseColorMapSampler;
layout( set = layoutSet_Model, binding = Model_image + 1) uniform samplerCube cubeMapSampler;
layout( set = layoutSet_Model, binding = Model_image + 2) uniform sampler2D	normalMapSampler;

//layout (input_attachment_index=0, set=layoutSet_Postprocessing, binding= Postprocessing_attach + 0) uniform subpassInput inputAttachment;
layout ( set=layoutSet_Postprocessing, binding= Postprocessing_attach + 0) uniform sampler2D inputAttachment;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTanget;
layout(location = 5) in vec3 inPostion;
layout(location = 6) in vec3 inLighttoVertex;
layout(location = 7) in vec3 inUV3;

layout(location = 0) out vec4 outColor;

// hdr, gamma, alpha
vec4 adjustColor( vec3 inColor, float alpha ){

	// reinhard tone mapping
    //inColor = inColor / (inColor + vec3(1.0));
	// Exposure tone mapping
    inColor = vec3(1.0) - exp(-inColor * environmentData.param.z);

	inColor *= modelData.mtl.metallicRoughnessEmissive.z;

	vec3 gamma = vec3(1.0/environmentData.param.y);
	return vec4(pow(inColor,gamma), alpha)*alpha;

	//return vec4(10,10,10,1);
}

bool isBloom( vec3 inColor ){
	// Bright parts for bloom
	return ( dot(inColor.rgb, vec3(0.2126, 0.7152, 0.0722)) > 0.9);
}