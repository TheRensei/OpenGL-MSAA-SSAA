#version 330


layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

uniform mat4 model;
uniform mat4 vpMatrix;

out vec4 colour;

void main(void) {

	colour = vertexColour;
	gl_Position = vpMatrix * model * vertexPos;
}
