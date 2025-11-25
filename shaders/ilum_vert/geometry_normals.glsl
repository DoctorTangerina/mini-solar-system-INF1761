#version 410

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_out {
    vec3 pos;
    vec3 n;
} v[];

uniform mat4 Vp;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        vec3 p = v[i].pos;
        vec3 n = v[i].n;
        vec3 end = p + n * 0.2;

        gl_Position = Vp * vec4(p, 1.0);
        EmitVertex();

        gl_Position = Vp * vec4(end, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}
