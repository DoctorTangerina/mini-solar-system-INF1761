#version 410

#define N 1

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoord;

uniform vec4 plane[N];
out float gl_ClipDistance[N];

uniform mat4 M;
uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;
uniform mat4 Mtex;

uniform vec4 lpos;  // light pos in eye space

out data {
  vec3 n;
  vec3 l;
  vec3 ve;
  vec3 t;
  vec2 texcoord;
  vec4 stexcoord;
} v;

void main (void) 
{
    vec3 neye = normalize(vec3(Mn*vec4(normal,0.0)));
    vec3 teye = normalize(vec3(Mn*vec4(tangent, 0.0)));
    
    vec3 veye = vec3(Mv*coord);
    for (int i = 0; i < N; ++i)
      gl_ClipDistance[i] = dot(plane[i],vec4(veye, 1.0f));

    vec3 light;

    if (lpos.w == 0)
        light = normalize(vec3(lpos));
    else
        light = normalize(vec3(lpos)-veye);

    veye = normalize(-veye);

    // passar dados para o fragment shader
    v.n = neye;
    v.l = light;
    v.ve = veye;
    v.t = teye;
    v.texcoord = texcoord;
    v.stexcoord = Mtex * M * coord;

    gl_Position = Mvp*coord;
}
