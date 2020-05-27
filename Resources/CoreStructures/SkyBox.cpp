#include "Skybox.h"

#include "ShaderLoader.h"
#include "TextureLoader.h"

using namespace std;

#pragma region Skybox model

// Packed vertex buffer for skybox model.  Vertces duplicated so each vertex on each face has its own normal
static float skyboxPositionArray[] = {

	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

#pragma endregion


Skybox::Skybox(const string& cubemapPath, const string& cubemapName, const string& cubemapExtension) {

	// Load shader
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("Resources\\Shaders\\Skybox_shader.vs"), string("Resources\\Shaders\\Skybox_shader.fs"), &skyboxShader);

	// Setup VAO
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &skyboxVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxPositionArray), skyboxPositionArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Unbind VAO when finished setup
	glBindVertexArray(0);

	//
	// Load cubemap texture
	//

#if 1

	// This version uses GL_MIRRORED_REPEAT to help eliminate seam edges in the cube map and skybox rendering

	texture = TextureLoader::loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_SRGB8_ALPHA8,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		false);

#else

	// This version uses GL_REPEAT which introduces cubemap sampling artefacts at the corners and edges of the skybox

	texture = TextureLoader::loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_REPEAT,
		GL_REPEAT,
		GL_REPEAT,
		false);

#endif


}


Skybox::~Skybox() {

	// Unbind VAO
	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Delete GL resources
	glDeleteBuffers(1, &skyboxVertexBuffer);
	glDeleteBuffers(1, &skyboxNormalVectorBuffer);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteShader(skyboxShader);
	glDeleteTextures(1, &texture);
}


GLuint Skybox::skyboxTexture() {

	return texture;
}


void Skybox::setSkyboxTexture(const GLuint newTexture) {

	if (newTexture == 0)
		return;

	if (texture)
		glDeleteTextures(1, &texture);

	texture = newTexture;
}


void Skybox::render(const glm::mat4& T) {

	static GLint mvpLocation = glGetUniformLocation(skyboxShader, "modelViewProjectionMatrix");

	glDepthMask(GL_FALSE);
	glUseProgram(skyboxShader);

	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(T));

	// Draw skybox directly - no indexing used
	glBindVertexArray(skyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Unbind VAO once drawing complete
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}
