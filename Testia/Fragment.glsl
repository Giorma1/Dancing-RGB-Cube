#version 460 core

out vec4 gl_FragColor;
in vec4 Colors;

uniform float time;


void main()
{
	gl_FragColor = Colors + (cos(time) + 1) * 2 / 2 * -0.1;
}