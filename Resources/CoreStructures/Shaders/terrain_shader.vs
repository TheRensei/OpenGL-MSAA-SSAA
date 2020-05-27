#version 330

uniform mat4 mvpMatrix;
uniform float tscale;
uniform float tyScale;

layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

out vec4 colour;

void main(void) {

	vec4 vPos = vec4(vertexPos.x * tscale, vertexPos.y * tyScale, vertexPos.z * tscale, 1.0);
	colour = vertexColour;
	gl_Position = mvpMatrix * vPos;
}
