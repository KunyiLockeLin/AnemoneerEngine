#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

void main() {

	vec3 view_vector =  normalize(inPostion - environmentData.camera.pos_rayTracingDepth.xyz);
  
	float angle = smoothstep( 0.3, 0.7, dot( normalize( -view_vector ), inNormal ) );
  
	vec3 reflect_vector = reflect( view_vector, inNormal );
	vec4 reflect_color = texture( cubeMapSampler, reflect_vector );
  
	vec3 refrac_vector = refract( view_vector, inNormal, 0.3 );
	vec4 refract_color = texture( cubeMapSampler, refrac_vector );
  
	vec4 cubemapColor = mix( reflect_color, refract_color, angle );
	outColor =  mix( outColor, cubemapColor, 0.7 );

	outColor = adjustColor(outColor.rgb, 1);
}