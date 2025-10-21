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

void main (void)
{
  vec3 neye = normalize(f.n);
  vec3 light = normalize(f.l);
  vec3 veye = normalize(f.ve);

  float ndotl = dot(neye,light);
  color = mamb*lamb + mdif * ldif * max(0,ndotl); 
  if (ndotl > 0) {
    vec3 refl = normalize(reflect(-light,neye));
    color += mspe * lspe * pow(max(0,dot(refl,normalize(-veye))),mshi); 
  }

  color = color * texture(decal,f.texcoord);
}

