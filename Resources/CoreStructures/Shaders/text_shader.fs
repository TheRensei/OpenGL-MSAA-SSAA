#version 330

uniform sampler2D texture0;
uniform vec3 textColor;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {

	vec4 texColor = vec4(1.0, 1.0, 1.0, texture(texture0, texCoord).r);
	
	fragColour = vec4(textColor, 1.0) * texColor;
}
