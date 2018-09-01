#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	vec4 texColor = texture(baseColorMapSampler, inUV);
	if( 0.01 > texColor.a )	discard;

	float lightType = lights[0].param.x;
	float lightIntensity = lights[0].param.y;
	float lightConeAngle = lights[0].param.z;

	float fDist2 = pow(length(inLighttoVertex),2);
	float attenuation  = lightIntensity/fDist2;

    vec3 vLighttoVertex;
	if( lightType == 0 )		vLighttoVertex = normalize(inLighttoVertex);
	
	else if( lightType == 1 )	vLighttoVertex = normalize(vec3(lights[0].dir));
	
	else if( lightType == 2 ){

		vLighttoVertex = normalize(inLighttoVertex);
		float lightToSurfaceAngle = degrees(acos( dot( -vLighttoVertex, normalize(vec3(lights[0].dir)) ) ));

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
	outColor = adjustColor(outColor.rgb, texColor.a);
}