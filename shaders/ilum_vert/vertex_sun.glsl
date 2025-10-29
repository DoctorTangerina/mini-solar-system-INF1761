#version 410

layout(location = 0) in vec4 coord;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mvp;

out data {
  vec2 texcoord;
} f;

void main (void) 
{
  f.texcoord = texcoord;
  gl_Position = Mvp*coord; 
}
