#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"
/*
// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(M_PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), modelData.mtl.baseColor.rgb, metallic); // * material.specular
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

// Specular BRDF composition --------------------------------------------
vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

void main()
{		  
	vec3 N = normalize(inNormal);
	vec3 V = normalize(environmentData.camera.pos.xyz - inPostion.xyz);

	float roughness = modelData.mtl.metallicRoughnessEmissive.y;

	// Add striped pattern to roughness based on vertex position
	roughness = max(roughness, step(fract(inPostion.y * 2.02), 0.5));

	// Specular contribution
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < environmentData.param.x; i++) {
		vec3 L = normalize(lights[i].pos.xyz - inPostion.xyz);
		Lo += BRDF(L, V, N, modelData.mtl.metallicRoughnessEmissive.x, roughness);
	};

	// Combine with ambient
	vec3 color = modelData.mtl.baseColor.rgb * 0.02;
	color += Lo;

	outColor = adjustColor(color, modelData.mtl.baseColor.a);
}
*/

void main() {
	vec4 baseColor1 = texture(baseColorMapSampler, inUV);
	if( 0.01 > baseColor1.a )	discard;


	float metallic = modelData.mtl.metallicRoughnessEmissive.x;	
    float perceptualRoughness = modelData.mtl.metallicRoughnessEmissive.y;

	// need metallicRoughness map
	const float c_MinRoughness = 0.04;
	perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
	metallic = clamp(metallic, 0.0, 1.0);
	float alphaRoughness = perceptualRoughness * perceptualRoughness;

	vec4 baseColor = baseColor1 * modelData.mtl.baseColor;

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
    vec3 tex_dx = dFdx(vec3(inUV, 0.0));
    vec3 tex_dy = dFdy(vec3(inUV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
	vec3 ng = normalize(inNormal);
	t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
	mat3 tbn = mat3(t, b, ng);
	// neeed normal map
	//vec3 n = tbn[2].xyz; 
    //vec3 n = texture(normalSampler, inUV).rgb;
    //n = normalize(tbn * ((2.0 * n - 1.0) * vec3(u_NormalScale, u_NormalScale, 1.0)));
	vec3 n = 2 * texture( normalMapSampler, inUV ).rgb - 1.0;
	n = normalize( mat3( inTangent, inBiTanget, inNormal) * n );

	float lightType		 = lights[0].param.x;
	float lightIntensity = lights[0].param.y;
	float lightConeAngle = lights[0].param.z;
	float fDist2		 = pow(length(inLighttoVertex),2);
	float attenuation	 = lightIntensity/fDist2;
	vec3  lightDirection;

	if( lightType == 0 )		lightDirection = normalize(inLighttoVertex);
	else if( lightType == 1 )	lightDirection = normalize(vec3(lights[0].dir));
	else if( lightType == 2 ){

		lightDirection = normalize(inLighttoVertex);
		float lightToSurfaceAngle = degrees(acos( dot( -inLighttoVertex, normalize(vec3(lights[0].dir)) ) ));

		if( lightToSurfaceAngle>lightConeAngle ) attenuation = 0;
	}

    vec3 v = normalize(environmentData.camera.pos.xyz - inPostion);
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
	vec3 color = NdotL * vec3(lights[0].color) * attenuation* (diffuseContrib + specContrib) + vec3(environmentData.ambientColor*baseColor)*0.1;

	// need image based lighting map
	// need occlusion map
	// need emissive map

	outColor = adjustColor(color, baseColor.a);

	// cubemap
	if(modelData.param.z == 0) return;

	vec3 view_vector =  normalize(inPostion - environmentData.camera.pos.xyz);
  
	float angle = smoothstep( 0.3, 0.7, dot( normalize( -view_vector ), n ) );
  
	vec3 reflect_vector = reflect( view_vector, n );
	vec4 reflect_color = texture( cubeMapSampler, reflect_vector );
  
	vec3 refrac_vector = refract( view_vector, n, 0.3 );
	vec4 refract_color = texture( cubeMapSampler, refrac_vector );
  
	vec4 cubemapColor = mix( reflect_color, refract_color, angle );
	outColor =  mix( outColor, cubemapColor, 0.7 );
}