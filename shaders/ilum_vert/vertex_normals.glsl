#version 410

layout(location = 0) in vec3 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mvp;
uniform mat4 Mn;

out VS_out {
    vec3 pos;
    vec3 n;
} v;

void main()
{
    v.pos = coord;
    v.n = normalize(mat3(Mn) * normal);
    gl_Position = Mvp * vec4(coord, 1.0);
}