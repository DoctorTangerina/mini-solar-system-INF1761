#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

uniform vec4 lpos;  // light pos in eye space

out vec3 n;
out vec3 l;
out vec3 v;

void main (void) 
{
  vec3 veye = vec3(Mv*coord);
  vec3 light;
  if (lpos.w == 0) 
    light = normalize(vec3(lpos));
  else 
    light = normalize(vec3(lpos)-veye); 
  vec3 neye = normalize(vec3(Mn*vec4(normal,0.0f)));
  
  n = neye;
  l = light;
  v = veye;
  gl_Position = Mvp*coord; 
}

