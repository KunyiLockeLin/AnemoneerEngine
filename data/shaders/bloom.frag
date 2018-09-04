#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"


void main() {
	
	//outColor = subpassLoad(inputAttachment);
	outColor = texture(inputAttachment, inUV);
	//if( outColor.r>1 || outColor.g>1 || outColor.b>1 ) outColor = vec4(1,0,0,1);

	const int horizontal= 0; // 0:horizontal, 1: vertical, 2: both
	const float blurScale = 0.003;
	const float blurStrength = 1.0;
	
	// Gaussian blur
	/*
	float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	vec2 tex_offset = 1.0 / textureSize(inputAttachment, 0)* blurScale; // gets size of single texel

	if( outColor.r>1 || outColor.g>1 || outColor.b>1 ) {
		outColor += outColor * weight[0]; // current fragment's contribution
	}

	if(horizontal != 1 )
	{
		for(int i = 1; i < 5; ++i)
		{
			vec4 color = texture(inputAttachment, inUV + vec2(tex_offset.x * i, 0.0));
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += color * weight[i] * blurStrength;
			}
			color = texture(inputAttachment, inUV - vec2(tex_offset.x * i, 0.0));
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += color * weight[i] * blurStrength;
			}
		}
	}

	if(horizontal != 0 )
	{
		for(int i = 1; i < 5; ++i)
		{  
			vec4 color = texture(inputAttachment, inUV + vec2(0.0, tex_offset.y * i));
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += color * weight[i] * blurStrength;
			}
			color = texture(inputAttachment, inUV - vec2(0.0, tex_offset.y * i));
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += color * weight[i] * blurStrength;
			}  
		}
	}*/
	
	// From the OpenGL Super bible
	const float weights[] = float[](0.0024499299678342,
									0.0043538453346397,
									0.0073599963704157,
									0.0118349786570722,
									0.0181026699707781,
									0.0263392293891488,
									0.0364543006660986,
									0.0479932050577658,
									0.0601029809166942,
									0.0715974486241365,
									0.0811305381519717,
									0.0874493212267511,
									0.0896631113333857,
									0.0874493212267511,
									0.0811305381519717,
									0.0715974486241365,
									0.0601029809166942,
									0.0479932050577658,
									0.0364543006660986,
									0.0263392293891488,
									0.0181026699707781,
									0.0118349786570722,
									0.0073599963704157,
									0.0043538453346397,
									0.0024499299678342);

	// Aspect ratio for vertical blur pass
	float ar = 1.0;
	const int dir = 1;
	if (dir == 1)
	{
		vec2 ts = textureSize(inputAttachment, 0);
		ar = ts.y / ts.x;
	}

	if(horizontal != 1 )
	{
		vec2 P = inUV.xy - vec2((weights.length() >> 1) * ar * blurScale, 0);
		for (int i = 0; i < weights.length(); i++)
		{
			vec2 dv = vec2(i * blurScale, 0.0) * ar;
			vec4 color = texture(inputAttachment, P + dv);
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += (color * weights[i] * blurStrength);
			}
		}
	}

	if(horizontal != 0 )
	{
		vec2 P = inUV.xy - vec2(0, (weights.length() >> 1) * ar * blurScale);
		for (int i = 0; i < weights.length(); i++)
		{
			vec2 dv = vec2(0.0, i * blurScale) * ar;
			vec4 color = texture(inputAttachment, P + dv);
			if( color.r>1 || color.g>1 || color.b>1 ) {
				outColor += (color * weights[i] * blurStrength);
			}
		}
	}
}