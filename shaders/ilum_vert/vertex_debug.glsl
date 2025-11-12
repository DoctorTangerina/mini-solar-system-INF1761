#version 410

layout(location = 0) in vec4 coord;
layout(location = 2) in vec2 texcoord;

out vec2 tex;

uniform mat4 Mvp;

void main()
{
    gl_Position = Mvp * coord;
    tex = texcoord;
}  