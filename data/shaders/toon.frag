#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_VIEWPORT_NUM = 9;
const int MAX_JOINT_NUM = 20;

layout( binding = 0) uniform QeUniformBufferObject {
	mat4 model;
    mat4 view[MAX_VIEWPORT_NUM];
    mat4 proj[MAX_VIEWPORT_NUM];
	mat4 normal[MAX_VIEWPORT_NUM];
	mat4 joints[MAX_JOINT_NUM];
	vec4 cameraPos[MAX_VIEWPORT_NUM];
	vec4 ambientColor;
	vec4 param; // 1: viewportNum, 2:billboardType
} ubo;

layout( binding = 1) uniform QeDataLight {
    vec4 pos;
    vec4 dir;
	vec4 color;
	vec4 param; // 1: type, 2: intensity, 3: coneAngle
	// type: 0: eLightPoint, 1: eLightDirection, 2: eLightSpot,

} light;

layout( binding = 2) uniform QeDataMaterial {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
	vec4 emissive;
	vec4 param; // 1: specularExponent, 2: refraction, 3: alpha,
} mtl;

layout( binding = 10) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inLighttoVertex;
layout(location = 4) in vec3 inEye;

layout(location = 0) out vec4 outColor;


void main() {
	vec4 texColor = texture(texSampler, inTexCoord);
	if( 0.01 > texColor.a ) {
		discard;
	}

	float mtlSpecularExponent = mtl.param.x;
	float mtlRefraction = mtl.param.y;
	float mtlAlpha = mtl.param.z;
	float lightType = light.param.x;
	float lightIntensity = light.param.y;
	float lightConeAngle = light.param.z;

	float fDist2 = pow(length(inLighttoVertex),2);
	float attenuation  = lightIntensity/fDist2;

    vec3 vLighttoVertex;
	if( lightType == 0 )		vLighttoVertex = normalize(inLighttoVertex);
	
	else if( lightType == 1 )	vLighttoVertex = normalize(vec3(light.dir));
	
	else if( lightType == 2 ){

		vLighttoVertex = normalize(inLighttoVertex);
		float lightToSurfaceAngle = degrees(acos( dot( -vLighttoVertex, normalize(vec3(light.dir)) ) ));

		if( lightToSurfaceAngle>lightConeAngle ) attenuation = 0;
	}
	
	float intensity = clamp(dot(inNormal,vLighttoVertex), 0, 1);

	vec3 color;
	if (intensity > 0.98)
		color = texColor.xyz * 1.5;
	else if  (intensity > 0.9)
		color = texColor.xyz * 1.0;
	else if (intensity > 0.5)
		color = texColor.xyz * 0.6;
	else if (intensity > 0.25)
		color = texColor.xyz * 0.4;
	else
		color = texColor.xyz * 0.2;

	color = vec3(mix(color, vec3(dot(vec3(0.2126,0.7152,0.0722), color)), 0.5f));
	outColor = vec4(color, texColor.a)*texColor.a;
}