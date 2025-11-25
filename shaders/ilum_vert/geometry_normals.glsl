#version 410

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_out {
    vec3 pos;
    vec3 n;
} v[];

uniform mat4 Mvp;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        vec3 p0 = v[i].pos;
        vec3 p1 = p0 + v[i].n * 0.2;     // direção da normal

        gl_Position = Mvp * vec4(p0, 1.0);
        EmitVertex();

        gl_Position = Mvp * vec4(p1, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}
