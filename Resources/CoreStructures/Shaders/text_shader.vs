#version 330

layout (location=0) in vec4 vertexPos;

uniform mat4 projection;

out vec2 texCoord;

void main(void) 
{
	texCoord = vertexPos.zw;
	
	gl_Position = projection * vec4(vertexPos.xy, 0.0, 1.0);
}
