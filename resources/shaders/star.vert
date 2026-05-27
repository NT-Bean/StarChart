#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;

out vec4 vertexColor;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;


void main()
{
	gl_Position = camMatrix * vec4(aPos, 1.0);
	vertexColor = vec4(color, 1.0f);
}