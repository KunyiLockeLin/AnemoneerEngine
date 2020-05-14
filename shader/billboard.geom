#version 450
#extension GL_GOOGLE_include_directive : enable
#include "header.geom"

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

void main(void) {
    outColor = inColor[0];

    vec3 size = vec3(modelData.model[0].x, modelData.model[1].y, modelData.model[2].z);

    if (size.x > 0 && size.y > 0 && size.z > 0) {
        mat4 model = modelData.model;
        model[3].xyz += gl_in[0].gl_Position.xyz;

        mat4 viewModel = environmentData.camera.view * model;
        viewModel[0].x = size.x;
        viewModel[0].y = 0;
        viewModel[0].z = 0;

        viewModel[1].x = 0;
        viewModel[1].y = size.y;
        viewModel[1].z = 0;

        viewModel[2].x = 0;
        viewModel[2].y = 0;
        viewModel[2].z = size.z;

        mat4 mvp = environmentData.camera.projection * viewModel;

        gl_Position = mvp * vec4(0.0, 0.0, 0.0, 1.0);
        outUV = vec2(0.0, 0.0);
        EmitVertex();

        gl_Position = mvp * vec4(0.0, -2.0, 0.0, 1.0);
        outUV = vec2(0.0, 2.0);
        EmitVertex();

        gl_Position = mvp * vec4(2.0, 0.0, 0.0, 1.0);
        outUV = vec2(2.0, 0.0);
        EmitVertex();
    }
    gl_ViewportIndex = gl_InvocationID;
    gl_PrimitiveID = gl_PrimitiveIDIn;

    EndPrimitive();
}
