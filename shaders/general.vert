#version 430 core

layout(location = 0) in vec2 inPos;

void main() {
    vec2 pos = fma(inPos, vec2(2.0f), vec2(-1.0f));
    gl_Position = vec4(pos, 0.0f, 1.0f);
}