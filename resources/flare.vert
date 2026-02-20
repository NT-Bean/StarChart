#version 330 core

layout (location = 0) in vec3 flarePos;
layout (location = 1) in vec4 color;

out vec4 vertexColor;

uniform mat4 camMatrix;
uniform float irradiance;
uniform float luminosity;

void main()
{
	gl_Position = camMatrix * vec4(flarePos, 1.0f);
	vertexColor = color;
	gl_PointSize = irradiance + max(7.0f * pow(luminosity, 1.0f / 3.0f), 5.0f); // * sqrt(luminosity);
}