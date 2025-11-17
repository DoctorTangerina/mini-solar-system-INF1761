#version 410

in vec3 n;
in vec3 l;
in vec3 v;
in vec4 stexcoord;

uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;
uniform float mopacity;

uniform sampler2DShadow smap;

out vec4 fcolor;

void main (void)
{
  vec3 neye = normalize(n);
  vec3 light = normalize(l);
  vec3 veye = normalize(v);

  float shadow = textureProj(smap, stexcoord);
  float ndotl = dot(neye,light);
  vec4 color = mamb*lamb + mdif * ldif * max(0,ndotl) * shadow; 
  if (ndotl > 0) {
    vec3 refl = normalize(reflect(-light,neye));
    color += mspe * lspe * pow(max(0,dot(refl,normalize(-veye))),mshi) * shadow; 
  }
  color.a = mopacity;
  fcolor = color;
}

