#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.frag"

void main() {
    // outColor = subpassLoad(inputAttachment);
    outColor = texture(inputAttachment, inUV);
}
