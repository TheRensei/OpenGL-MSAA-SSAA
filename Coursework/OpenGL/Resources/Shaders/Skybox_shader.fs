#version 330

// The skybox is rendered with the same texture as the environment map (cubemap) used to calculate reflections.
// Therefore use the same cubemap to avoid duplicating resources.
// To do this without reflection-based distortion simply use the axis-aligned normals of each skybox face to lookup the 
// appropriate cubemap.

uniform samplerCube cubemap;

in vec3 TexCoords;

layout (location = 0) out vec4 fragColour;

void main(void)
{
	fragColour = texture(cubemap, TexCoords);
}
