#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

out data {
  vec3 n;
  vec3 l;
  vec3 ve;
  vec2 texcoord;
} v;

void main (void) 
{
  vec3 veye = vec3(Mv*coord);
  vec3 light;
  if (lpos.w == 0) 
    light = normalize(vec3(lpos));
  else 
    light = normalize(vec3(lpos)-veye); 
  vec3 neye = normalize(vec3(Mn*vec4(normal,0.0f)));
  
  v.n = neye;
  v.l = light;
  v.ve = veye; 
  v.texcoord = texcoord;
  gl_Position = Mvp*coord; 
}

