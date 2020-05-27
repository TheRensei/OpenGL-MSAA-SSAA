#include "AABB.h"


static float boxPositionArray[] = 
{
	-1.0, -1.0, -1.0, 
	 1.0, -1.0, -1.0, 
	 1.0,  1.0, -1.0, 
	-1.0,  1.0, -1.0, 
	-1.0, -1.0,  1.0, 
	 1.0, -1.0,  1.0, 
	 1.0,  1.0,  1.0, 
	-1.0,  1.0,  1.0 
};

GLushort elements[] = {
  0, 1, 2, 3,
  4, 5, 6, 7,
  0, 4, 1, 5, 2, 6, 3, 7
};

void AABB::setupVAO()
{	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxPositionArray), boxPositionArray, GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glBindVertexArray(0);
}

void AABB::setViewProjection(glm::mat4 viewProjection)
{
	this->viewProjectionMatrix = viewProjection;
}

bool AABB::checkCollision(glm::vec4 frustumPlanes[])
{
	for(int i = 0; i < 6; i++)
	{
		float dist = frustumPlanes[i].x * this->position.x +
			frustumPlanes[i].y * this->position.y +
			frustumPlanes[i].z * this->position.z +
			frustumPlanes[i].w - this->size.x;

		if (dist > 0)
		{
			this->visible = true;
			return false;
		}
	}

	this->visible = false;
	return true;
}

AABB::AABB(glm::vec3 size)
{
	this->size = size;
	this->visible = true;
	setupVAO();
}

AABB::AABB(GLuint VAO, glm::vec3 size)
{
	this->size = size;
	this->visible = true;
	this->VAO = VAO;
}

void AABB::render(GLuint lineShader, bool frustum)
{
	static GLint cLocation = glGetUniformLocation(lineShader, "colour");
	static GLint mLocation = glGetUniformLocation(lineShader, "model");
	static GLint vpLocation = glGetUniformLocation(lineShader, "viewProjection");

	glUseProgram(lineShader);

	glm::vec4 colour(1.0, 1.0, 1.0, 1.0); //white by default

	if (!this->visible)
		colour = glm::vec4(1.0, 0.0, 0.0, 1.0); //red

	if (frustum)
		colour = glm::vec4(0.1, 1.0, 0.1, 1.0); //green frustum outline

	glUniform4fv(cLocation, 1, glm::value_ptr(colour));
	glUniformMatrix4fv(mLocation, 1, GL_FALSE, glm::value_ptr(this->modelMatrix));
	glUniformMatrix4fv(vpLocation, 1, GL_FALSE, glm::value_ptr(this->viewProjectionMatrix));

	glBindVertexArray(this->VAO);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	glBindVertexArray(0);

	glUseProgram(0);
}

glm::vec3 AABB::getSize()
{
	return this->size;
}

void AABB::setPosition(glm::vec3 position)
{
	this->position = position;
	this->modelMatrix = glm::translate(glm::mat4(1.0), this->position);
}

glm::vec3 AABB::getPosition()
{
	return this->position;
}

glm::mat4 AABB::getModelMatrix()
{
	return this->modelMatrix;
}

void AABB::setModelMatrix(glm::mat4 modelMat)
{
	this->modelMatrix = modelMat;
}
