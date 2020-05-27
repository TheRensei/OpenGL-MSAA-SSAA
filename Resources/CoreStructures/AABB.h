#ifndef AABB_H
#define AABB_H
#include <Model.h>
#include "ShaderLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// model basic Axis aligned bounding box (AABB)

class AABB {
private:
	GLuint	VAO;
	GLuint	VBO;
	GLuint	EBO;
	bool	visible;

	glm::vec3 position;
	glm::vec3 size;

	glm::mat4 modelMatrix;
	glm::mat4 scaleMatrix;
	glm::mat4 viewProjectionMatrix;

	void setupVAO();
	void loadShader();


public:
	Model *model;

	AABB(glm::vec3 size = glm::vec3(1.0));
	AABB(GLuint VAO, glm::vec3 size);

	glm::mat4 getModelMatrix();
	void setModelMatrix(glm::mat4 model);

	void setViewProjection(glm::mat4 viewProjection);
	glm::vec3 getSize();
	void setSize(glm::vec3 size);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);

	void render(GLuint lineShader, bool frustum = false);

	bool checkCollision(glm::vec4 frustumPlanes[]);
};

#endif