#version 410

in vec4 stexcoord;

out vec4 color;

uniform sampler2DShadow smap;

void main (void) {
	float depthValue = textureProj(smap, stexcoord);
    color = vec4(vec3(depthValue), 1.0);
}