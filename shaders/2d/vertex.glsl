#version 410

layout (location=0) in vec4 vertex;
layout (location=1) in vec2 texcoord;

out data {
    vec2 texcoord;
} v;

uniform mat4 Mvp;

void main (void)
{
  gl_Position = Mvp * vertex;
  v.texcoord = texcoord;
}

