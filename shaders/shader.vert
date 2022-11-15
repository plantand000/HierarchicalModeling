#version 120

attribute vec3 position;
attribute vec3 color;
varying vec3 fragColor;
uniform mat4 mvp;


void main()
{
	gl_Position = mvp * vec4(position, 1.0);
	fragColor = color;
}
