#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

/*
samplerCube samplerIrradiance;
sampler2D samplerBRDFLUT;
samplerCube prefilteredMap;
image based lighting map
occlusion map
emissive map
*/

vec3 Diffuse_Lambertian( vec3 baseColor ){
    return baseColor / M_PI;
}

float D_TrowbridgeReitzGGX(float dotNH, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float d = dotNH * dotNH * (a2 - 1.0) + 1.0;
	return (a2)/(M_PI * d*d); 
}

float GK_Direct(float roughness) {
	float r = (roughness + 1.0);
	return (r*r) / 8.0;
}

float GK_IBL(float roughness) {
	return (roughness*roughness) / 2.0;
}

float G_SmithSchlickGGX(float dotNL, float dotNV, float roughness) {
	float k = GK_Direct(roughness);
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

vec3 F_FresnelSchlick(float dotVH, vec3 F0) {
	return (F0 + (1.0 - F0) * pow(1.0 - dotVH, 5.0));    
}

vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness, vec3 baseColor)
{
	vec3 H = normalize (V + L);
	float dotNV = clamp(abs(dot(N, V)), 0.001, 1.0);
	float dotNL = clamp(dot(N, L), 0.001, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotVH = clamp(dot(V, H), 0.0, 1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0) {

		// Cook-Torrance
		float D = D_TrowbridgeReitzGGX(dotNH, roughness); 
		float G = G_SmithSchlickGGX(dotNL, dotNV, roughness);
		vec3 F = F_FresnelSchlick(dotVH, F0);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);
		vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
		color = (kD * Diffuse_Lambertian(baseColor) + spec) * dotNL;
	}

	return color;
}

vec3 getNormal(){

	/*mat3 TBN = mat3(inTangent, inBiTanget, inNormal);
	vec3 N = normalize(TBN[2].xyz);
	if(modelData.param1.y == 1) {
		N = texture(normalMapSampler, inUV).rgb;
		float u_NormalScale = 1.0;
		N = normalize(TBN * ((2.0 * N - 1.0)*vec3(u_NormalScale, u_NormalScale, 1.0)));
	}*/

	vec3 pos_dx = dFdx(inPostion);
    vec3 pos_dy = dFdy(inPostion);
    vec3 tex_dx = dFdx(vec3(inUV, 0.0));
    vec3 tex_dy = dFdy(vec3(inUV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
	vec3 ng = normalize(inNormal);
	t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 TBN = mat3(t, b, ng);
	vec3 N = normalize(TBN[2].xyz);
	if(modelData.param1.y == 1) {
		N = texture(normalMapSampler, inUV).rgb;
		float u_NormalScale = 1.0;
		N = normalize(TBN * ((2.0 * N - 1.0) * vec3(u_NormalScale, u_NormalScale, 1.0)));
	}
	return N;
}

void main() {
	
	vec4 baseColor = modelData.mtl.baseColor;
	if(modelData.param1.x == 1) {
		vec4 color = texture(baseColorMapSampler, inUV);
		if( 0.01 > color.a )	discard;
		baseColor *= color;
	}

	float metallic = clamp( modelData.mtl.metallicRoughnessEmissive.x, 0.0, 1.0);	
    float roughness = clamp( modelData.mtl.metallicRoughnessEmissive.y, 0.04, 1.0);

	if(modelData.param1.w == 1) {
		vec4 value = texture(metallicRoughnessMapSampler, inUV);
		metallic *= value.x;
		roughness *= value.y;
	}

	vec3 N = getNormal();
	vec3 V = normalize(environmentData.camera.pos_rayTracingDepth.xyz - inPostion);
	vec3 F0 = mix(vec3(0.04), modelData.mtl.baseColor.rgb, metallic);

	vec3 color = baseColor.rgb*0.1;
	for (int i = 0; i < environmentData.param.x; i++) {

		vec3 L = lights[i].pos.xyz-inPostion;

		float lightType		 = lights[i].param.x;
		float lightIntensity = lights[i].param.y;
		float lightConeAngle = lights[i].param.z;
		float fDist2		 = pow(length(L),2);
		float attenuation	 = lightIntensity/fDist2;
		
		if( lightType == 1 )	L = normalize(vec3(lights[i].dir));
		else if( lightType == 2 ){

			float lightToSurfaceAngle = degrees(acos( dot( -L, normalize(vec3(lights[i].dir)) ) ));
			if( lightToSurfaceAngle>lightConeAngle ) attenuation = 0;
		}
		L = normalize(L);

		color += (BRDF(L, V, N, F0, metallic, roughness, baseColor.rgb)*vec3(lights[i].color) * attenuation);
	};

	// cubemap
	if(modelData.param1.z == 1) {

		vec3 view_vector =  normalize(inPostion - environmentData.camera.pos_rayTracingDepth.xyz);
  
		float angle = smoothstep( 0.3, 0.7, dot( normalize( -view_vector ), N ) );
  
		vec3 reflect_vector = reflect( view_vector, N );
		vec3 reflect_color = texture( cubeMapSampler, reflect_vector ).rgb;
  
		vec3 refrac_vector = refract( view_vector, N, 0.3 );
		vec3 refract_color = texture( cubeMapSampler, refrac_vector ).rgb;
  
		vec3 cubemapColor = mix( reflect_color, refract_color, angle );
		color =  mix( color, cubemapColor, 0.8 );
	}
	outColor = adjustColor(color, baseColor.a);
}