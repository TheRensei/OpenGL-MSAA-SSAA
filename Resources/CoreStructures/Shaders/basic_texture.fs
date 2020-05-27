#version 330

uniform sampler2D texture0;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {

	vec4 texColor = texture(texture0, texCoord);
	fragColour = texColor;
}

