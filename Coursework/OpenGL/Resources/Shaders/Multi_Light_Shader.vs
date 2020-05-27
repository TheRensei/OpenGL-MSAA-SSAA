#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

layout(std140) uniform cameraMatrixBlock
{
	mat4 model;
	mat4 viewProjection;
	mat4 invTransposeModelMatrix; // inverse transpose of model matrix to transform normal vector into world coords
	vec4 viewPosition;
} camera;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 viewPos;

void main()
{
	FragPos = vec3(camera.model * vec4(vertexPos, 1.0));
	Normal = mat3(camera.invTransposeModelMatrix) * vertexNormal;
	viewPos = vec3(camera.viewPosition.xyz);
	TexCoord = texCoord;
	
	gl_Position = camera.viewProjection * vec4(FragPos, 1.0f);
}