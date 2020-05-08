#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

void main() {
    vec4 texColor = texture(baseColorMapSampler, inUV);
    if (0.01 > texColor.a) discard;

    outColor = texColor * modelData.mtl.baseColor * texColor.a;
    outColor = adjustColor(outColor.rgb, texColor.a);
}
