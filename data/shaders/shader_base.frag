#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( binding = 1) uniform sampler2D texSampler;

layout( binding = 2) uniform QeDataLight {
    vec4 pos;
    vec4 dir;
	vec4 color;
	int type; //	0: eLightPoint, 1: eLightDirection, 2: eLightSpot,
	float intensity;
	float coneAngle;
} light;

layout( binding = 3) uniform QeDataMaterial {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	vec4 emissive;
	float refraction;
	float specularExponent;
	float alpha;
} mtl;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragLighttoVertex;
layout(location = 4) in vec3 fragEye;

layout(location = 0) out vec4 outColor;


void main() {
	
	float fDist2 = pow(length(fragLighttoVertex),2);
	float attenuation  = light.intensity/fDist2;

    vec3 vLighttoVertex;
	if( light.type == 0 )		vLighttoVertex = normalize(fragLighttoVertex);
	
	else if( light.type == 1 )	vLighttoVertex = normalize(vec3(light.dir));
	
	else if( light.type == 2 ){

		vLighttoVertex = normalize(fragLighttoVertex);
		float lightToSurfaceAngle = degrees(acos( dot( -vLighttoVertex, normalize(vec3(light.dir)) ) ));

		if( lightToSurfaceAngle>light.coneAngle ) attenuation = 0;
	}

	float cosTheta = clamp(dot(fragNormal,vLighttoVertex), 0, 1);

	vec4 texColor = texture(texSampler, fragTexCoord);

	vec4 ambient =  mtl.ambient*texColor;
	vec4 diffuse = mtl.diffuse*texColor*cosTheta;

	vec3 E = normalize(fragEye);
	vec3 R = reflect(-vLighttoVertex,fragNormal);
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	vec4 specular =  mtl.specular*pow(cosAlpha,mtl.specularExponent);

	vec4 gamma = vec4(1.0/2.2);
	gamma.w = 1;

	outColor =	pow( ambient + (diffuse + specular)*light.color*attenuation, gamma) ;
}