// Circle.h - A class to create circle VAO
#ifndef CIRCLE_H
#define CIRCLE_H

#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>

class Circle
{

private:
	GLuint			modelVAO;

	GLuint			meshPositionVBO;
	GLuint			meshColourVBO;
	GLuint			meshNormalVBO;
	GLuint			meshIndexVBO;

	GLuint			numElements;
public:
	Circle(int numVertices = 360, float radius = 1.0f);
	~Circle();

	void render(); // encapsulate rendering with VBOs

};

#endif