#version 330

in vec4 colour;

layout (location=0) out vec4 fragColour;

void main(void) {

	fragColour = vec4(0.5, 0.5, 0.5, 1.0);
}
