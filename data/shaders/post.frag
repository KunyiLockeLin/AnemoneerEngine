#version 450
#extension GL_ARB_separate_shader_objects : enable


layout (binding = 3) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

void main() 
{
	outColor = texture(samplerColor, inUV);
	outColor.r = (outColor.r+outColor.g +outColor.b)/3; 
	outColor.g = outColor.r;
	outColor.b = outColor.r; 
}