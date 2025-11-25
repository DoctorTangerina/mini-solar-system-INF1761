#version 410

layout(location = 0) in vec3 coord;
layout(location = 1) in vec3 normal;

uniform mat4 M;
uniform mat4 Mn;

out VS_out {
    vec3 pos;
    vec3 n;
} v;

void main()
{
    v.pos = vec3(M * vec4(coord, 1.0));
    v.n = normalize(normal);
    gl_Position = vec4(v.pos, 1.0);
}