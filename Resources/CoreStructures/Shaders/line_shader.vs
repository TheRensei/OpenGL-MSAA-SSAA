#version 330

uniform mat4 model;
uniform mat4 viewProjection;

layout (location=0) in vec3 vertexPos;

void main(void) 
{
	gl_Position = viewProjection * model * vec4(vertexPos, 1.0);
}
