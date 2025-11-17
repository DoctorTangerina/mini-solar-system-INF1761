#version 410

layout(location = 0) in vec4 coord;

out vec4 stexcoord;

uniform mat4 M;
uniform mat4 Mvp;
uniform mat4 Mtex;

void main(void) {
	stexcoord = Mtex * M * coord;
	gl_Position = Mvp * coord;
}
