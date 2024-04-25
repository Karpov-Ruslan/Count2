#version 430 core

layout(location = 0) out vec4 outColor;
layout(location = 0) uniform float color;

void main() {
    outColor = vec4(vec3(color), 1.0f);
}