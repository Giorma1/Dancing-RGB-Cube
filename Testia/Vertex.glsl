#version 460 core

in layout (location = 0) vec4 aPos;
in layout (location = 1) vec4 aColor;

out vec4 Colors;

uniform mat4 projection, model, view;

void main() 
{
	gl_Position = projection * view * model * aPos;
	Colors = aColor;
}