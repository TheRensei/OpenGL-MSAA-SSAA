#include "ScreenAlignedQuad.h"

using namespace std;

// Geometry data for textured quad (this is rendered directly as a triangle strip)

static float quadPositionArray[] = {

	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f
};

static float quadTextureCoordArray[] = {

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
};

ScreenAlignedQuad::ScreenAlignedQuad() {

	// Setup VAO for textured quad object
	glGenVertexArrays(1, &quadVertexArrayObj);
	glBindVertexArray(quadVertexArrayObj);


	// Setup VBO for position attribute
	glGenBuffers(1, &quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositionArray), quadPositionArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	// Setup VBO for texture coord attribute
	glGenBuffers(1, &quadTextureCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadTextureCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordArray), quadTextureCoordArray, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, nullptr);

	// Enable vertex buffers for textured quad rendering (vertex positions and colour buffers)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	// Unbind textured quad VAO
	glBindVertexArray(0);
}


ScreenAlignedQuad::~ScreenAlignedQuad() {

	// Unbind textured quad VAO
	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &quadVertexBuffer);
	glDeleteBuffers(1, &quadTextureCoordBuffer);

	glDeleteVertexArrays(1, &quadVertexArrayObj);
}


void ScreenAlignedQuad::render() {

	glBindVertexArray(quadVertexArrayObj);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}
