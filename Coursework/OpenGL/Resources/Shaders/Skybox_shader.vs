#version 330

// The skybox is rendered with the same texture as the environment map (cubemap) used to calculate reflections.
// Therefore use the same cubemap to avoid duplicating resources.
// To do this without reflection-based distortion simply use the axis-aligned normals of each skybox face to lookup the 
// appropriate cubemap face.

uniform mat4 modelViewProjectionMatrix;

layout (location = 0) in vec3 vertexPos;

out vec3 TexCoords;

void main(void)
{
	TexCoords = vertexPos;

	gl_Position = modelViewProjectionMatrix * vec4(vertexPos, 1.0);
}
