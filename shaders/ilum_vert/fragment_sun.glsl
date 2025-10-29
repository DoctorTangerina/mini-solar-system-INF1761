#version 410

in data {
  vec2 texcoord;
} f;

uniform sampler2D decal;

out vec4 fcolor;

void main (void)
{
  fcolor = texture(decal, f.texcoord);
}
