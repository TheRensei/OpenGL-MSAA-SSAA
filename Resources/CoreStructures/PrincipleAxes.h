#ifndef PRINCIPLE_AXES_H
#define PRINCIPLE_AXES_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

class PrincipleAxes {

private:

	GLuint					paVertexArrayObj;

	GLuint					paVertexBuffer;
	GLuint					paColourBuffer;

	GLuint					paIndexBuffer;

	GLuint					paShader;

public:

	PrincipleAxes();

	~PrincipleAxes();

	void render(const glm::mat4& T);
};

#endif