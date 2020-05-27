#version 330

uniform mat4 mvpMatrix;

layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

out vec4 colour;

void main(void) {

	colour = vertexColour;
	gl_Position = mvpMatrix * vertexPos;
}
