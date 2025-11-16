#version 410

in data {
  vec3 n;
  vec3 l;
  vec3 ve;
  vec3 t;
  vec2 texcoord;
  vec4 stexcoord;
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
uniform sampler2DShadow smap;

// converte [0,1] -> [-1,1]
vec3 expand (vec3 v)
{
  return (v-0.5) * 2.0;
}

void main (void)
{
    vec3 tang = normalize(f.t);
    vec3 norm = normalize(f.n);
    tang = normalize(tang - dot(tang, norm) * norm);

    vec3 binorm = cross(tang, norm);

    mat3 TBN = mat3(tang, binorm, norm);
    vec3 normal = normalize(TBN * expand(texture(normal, f.texcoord).rgb));

    //vec3 normal = normalize(f.n + 0.5 * normalMapVal);

    vec3 light = normalize(f.l);
    vec3 veye = normalize(f.ve);

    color = mamb * lamb;

    float ndotl = max(dot(normal, light), 0.0);
    float shadow = textureProj(smap, f.stexcoord);
    color += mdif * ldif * ndotl * shadow;

    if (ndotl > 0.0) {
        vec3 refl = normalize(reflect(-light, normal));
        color += mspe * lspe * shadow * pow(max(dot(refl, veye),0.0), mshi);
    }

    color *= texture(decal, f.texcoord);
}
