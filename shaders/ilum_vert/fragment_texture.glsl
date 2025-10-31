#version 410

in data {
  vec3 n;
  vec3 l;
  vec3 ve;
  mat3 tbn;
  vec2 texcoord;
} f;

uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;

out vec4 color;

uniform sampler2D decal;
uniform sampler2D normal;

// converte [0,1] -> [-1,1]
vec3 expand (vec3 v)
{
  return (v-0.5) * 2.0;
}

void main (void)
{
    vec3 normal = normalize(f.tbn * expand(texture(normal, f.texcoord).rgb));

    //vec3 normal = normalize(f.n + 0.5 * normalMapVal);

    vec3 light = normalize(f.l);
    vec3 veye = normalize(f.ve);

    color = mamb * lamb;

    float ndotl = max(dot(normal, light), 0.0);
    color += mdif * ldif * ndotl;

    if (ndotl > 0.0) {
        vec3 refl = normalize(reflect(-light, normal));
        color += mspe * lspe * pow(max(dot(refl, veye),0.0), mshi);
    }

    color *= texture(decal, f.texcoord);
}
