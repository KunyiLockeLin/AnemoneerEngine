//	http://github.khronos.org/glTF-WebGL-PBR/
//	https://github.com/KhronosGroup/glTF-WebGL-PBR/blob/master/shaders/pbr-frag.glsl
//	http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//	http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
//	https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
//	https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf

#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

void main() {
	vec4 baseColor1 = texture(baseColorMapSampler, inUV);
	if( 0.01 > baseColor1.a )	discard;

	float metallic = modelData.mtl.metallicRoughness.x;	
    float perceptualRoughness = modelData.mtl.metallicRoughness.y;

	// need metallicRoughness map
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

	vec3 gamma = vec3(1.0/2.2);
	//outColor = vec4(pow(color,gamma), baseColor.a);
	outColor = vec4(pow(color,gamma), baseColor.a)*baseColor.a;

	// cubemap
	/*if(modelData.param.x == 0) return;

	vec3 view_vector =  normalize(inPostion - environmentData.camera.pos.xyz);
  
	float angle = smoothstep( 0.3, 0.7, dot( normalize( -view_vector ), n ) );
  
	vec3 reflect_vector = reflect( view_vector, n );
	vec4 reflect_color = texture( cubeMapSampler, reflect_vector );
  
	vec3 refrac_vector = refract( view_vector, n, 0.3 );
	vec4 refract_color = texture( cubeMapSampler, refrac_vector );
  
	vec4 cubemapColor = mix( reflect_color, refract_color, angle );
	outColor =  mix( outColor, cubemapColor, 0.7 );*/
}
