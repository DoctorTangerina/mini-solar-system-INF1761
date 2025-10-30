#version 410

in data {
  vec3 n;
  vec3 l;
  vec3 ve;
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
uniform sampler2D normalMap;

vec3 expand (vec3 v)
{
  return (v-0.5) * 2;
}

void main (void)
{
  vec3 light = normalize(f.l);
  vec3 normal = expand(vec3(texture(normalMap, f.texcoord)));
  vec3 veye = normalize(f.ve);

  if (light.z > 0)
  {
    float ndotl = dot(normal,light);
    color = mamb*lamb + mdif * ldif * max(0,ndotl); 

    if (ndotl > 0) {
      vec3 refl = normalize(reflect(-light,normal));
      color += mspe * lspe * pow(max(0,dot(refl,normalize(-veye))),mshi); 
    } 
  }
  else
    color = mamb*lamb;
  

  color = color * texture(decal,f.texcoord);
}

