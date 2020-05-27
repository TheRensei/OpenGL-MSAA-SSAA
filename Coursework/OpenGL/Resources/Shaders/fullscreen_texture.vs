#version 330

uniform mat4 mvpMatrix;

layout (location=0) in vec4 vertexPos;
layout (location=2) in vec2 vertexTexCoord;

out vec2 texCoord;

void main(void) 
{
	texCoord = vertexTexCoord;
	gl_Position = vertexPos;
}
