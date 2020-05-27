#ifndef SCREEN_ALIGNED_QUAD_H
#define SCREEN_ALIGNED_QUAD_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

class ScreenAlignedQuad 
{

private:

	GLuint					quadVertexArrayObj;

	GLuint					quadVertexBuffer;
	GLuint					quadTextureCoordBuffer;

public:

	ScreenAlignedQuad();
	~ScreenAlignedQuad();

	void render();
};

#endif