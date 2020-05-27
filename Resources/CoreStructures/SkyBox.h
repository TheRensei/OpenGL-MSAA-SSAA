#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class ShaderLoader;
class TextureLoader;


class Skybox {

private:

	GLuint					skyboxVAO;

	GLuint					skyboxVertexBuffer;
	GLuint					skyboxNormalVectorBuffer;

	GLuint					skyboxShader;

	GLuint					texture;

public:

	Skybox(const std::string& cubemapPath, const std::string& cubemapName, const std::string& cubemapExtension);
	~Skybox();

	GLuint skyboxTexture();
	void setSkyboxTexture(const GLuint newTexture);

	void render(const glm::mat4& T);
};

#endif