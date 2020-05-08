#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

void main() {
    vec4 baseColor = modelData.mtl.baseColor;
    if (modelData.param1.z == 1) {
        vec4 color = texture(cubeMapSampler, inUV3);
        if (0.01 > color.a) discard;
        baseColor *= color;
    }
    outColor = adjustColor(baseColor.rgb, 1);
}
