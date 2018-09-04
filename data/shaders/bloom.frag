#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	
	//outColor = subpassLoad(inputAttachment);
	outColor = texture(inputAttachment, inUV);
	//if( outColor.r>1 || outColor.g>1 || outColor.b>1 ) outColor = vec4(1,0,0,1);

	// Gaussian blur  
/*
bool horizontal=true;
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec2 tex_offset = 1.0 / textureSize(inputAttachment, 0); // gets size of single texel
vec3 result = texture(inputAttachment, inUV).rgb * weight[0]; // current fragment's contribution
vec3 color;

if(horizontal)
{
    for(int i = 1; i < 5; ++i)
    {
		color = texture(inputAttachment, inUV + vec2(tex_offset.x * i, 0.0)).rgb;
		if( color.r>1 || color.g>1 || color.b>1 ) {
			result += color * weight[i];
        }
		color = texture(inputAttachment, inUV - vec2(tex_offset.x * i, 0.0)).rgb;
		if( color.r>1 || color.g>1 || color.b>1 ) {
			result += color * weight[i];
        }
    }
}
else
{
    for(int i = 1; i < 5; ++i)
    {  
		color = texture(inputAttachment, inUV + vec2(0.0, tex_offset.y * i)).rgb;
		if( color.r>1 || color.g>1 || color.b>1 ) {
			result += color * weight[i];
        }
		color = texture(inputAttachment, inUV - vec2(0.0, tex_offset.y * i)).rgb;
		if( color.r>1 || color.g>1 || color.b>1 ) {
			result += color * weight[i];
        }  
	}
}
outColor = vec4(result, 1.0);
*/
}