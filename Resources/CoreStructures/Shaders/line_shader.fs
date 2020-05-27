#version 330

uniform vec4 colour;

layout (location=0) out vec4 fragColour;

void main(void) {

	fragColour = colour;
}
