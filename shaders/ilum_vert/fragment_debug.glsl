#version 410

in vec2 texcoord;

out vec4 fcolor;

uniform sampler2D smap;
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) /
           (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depthValue = texture(smap, texcoord).r;
    float linearDepth = LinearizeDepth(depthValue) / far_plane; // normaliza 0..1
    fcolor = vec4(vec3(linearDepth), 1.0);
}