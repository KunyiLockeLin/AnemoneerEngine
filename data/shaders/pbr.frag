//	https://github.com/KhronosGroup/glTF-WebGL-PBR/blob/master/shaders/pbr-frag.glsl
//	http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//	http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
//	https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
//	https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf

#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_VIEWPORT_NUM = 9;

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
	// type: 0: eLightPoint, 1: eLightDirection, 2: eLightSpot,
} light;

layout( binding = 2) uniform QeDataPBRMaterial {
	vec4 baseColor;
	vec4 metallicRoughness;
	vec4 emissive;
} mtl;

layout( binding = 3) uniform sampler2D baseColorSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTanget;
layout(location = 5) in vec3 inPostion;
layout(location = 6) in vec3 inCameraPostion;

layout(location = 0) out vec4 outColor;

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;


void main() {
	
	float metallic = mtl.metallicRoughness.x;	
    float perceptualRoughness = mtl.metallicRoughness.y;

	// need metallicRoughness map
	perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
	metallic = clamp(metallic, 0.0, 1.0);
	float alphaRoughness = perceptualRoughness * perceptualRoughness;

	vec4 baseColor = texture(baseColorSampler, inTexCoord) * mtl.baseColor;

	vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= (1.0 - metallic);
    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	// getNormal
	vec3 pos_dx = dFdx(inPostion);
    vec3 pos_dy = dFdy(inPostion);
    vec3 tex_dx = dFdx(vec3(inTexCoord, 0.0));
    vec3 tex_dy = dFdy(vec3(inTexCoord, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
	vec3 ng = normalize(inNormal);
	t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
	mat3 tbn = mat3(t, b, ng);
	// neeed normal map
	vec3 n = tbn[2].xyz; 

	vec3 lightDirection;
	if( light.param.x == 0 || light.param.x == 2 )	lightDirection = normalize(vec3(light.pos)-inPostion);
	else if( light.param.x == 1 )					lightDirection = normalize(vec3(light.dir));

    vec3 v = normalize(inCameraPostion - inPostion);
    vec3 l = normalize(lightDirection);    
    vec3 h = normalize(l+v);
    vec3 reflection = -normalize(reflect(v, n));

    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = abs(dot(n, v)) + 0.001;
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
	float VdotH = clamp(dot(v, h), 0.0, 1.0);

	// specularReflection
	vec3 F = specularEnvironmentR0 + (specularEnvironmentR90 - specularEnvironmentR0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
	
	// geometricOcclusion
	float roughnessSq = alphaRoughness * alphaRoughness;
    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(roughnessSq + (1.0 - roughnessSq) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(roughnessSq + (1.0 - roughnessSq) * (NdotV * NdotV)));
	float G = attenuationL * attenuationV;
	
	// microfacetDistribution
    float f1 = (NdotH * roughnessSq - NdotH) * NdotH + 1.0;
	float D = roughnessSq / (M_PI * f1 * f1);

	// Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuseColor / M_PI;
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
	vec3 color = NdotL * vec3(light.color) * (diffuseContrib + specContrib);

	// need image based lighting map
	// need occlusion map
	// need emissive map

	vec3 gamma = vec3(1.0/2.2);
	outColor = vec4(pow(color,gamma), baseColor.a);
}