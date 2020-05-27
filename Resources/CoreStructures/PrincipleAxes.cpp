#include "PrincipleAxes.h"
#include "ShaderLoader.h"


using namespace std;

// Example data for principle axes

// Packed vertex buffer for principle axes model
static float paPositionArray[] = {

	// x
	0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.025f, 0.025f, 0.0, 1.0f,
	1.075f, 0.075f, 0.0, 1.0f,
	1.025f, 0.075f, 0.0, 1.0f,
	1.075f, 0.025f, 0.0, 1.0f,

	// y
	0.0, 0.0, 0.0, 1.0f,
	0.0, 1.0, 0.0, 1.0f,
	-0.075f, 1.075f, 0.0, 1.0f,
	-0.05f, 1.05f, 0.0, 1.0f,
	-0.025f, 1.075f, 0.0, 1.0f,
	-0.075f, 1.025f, 0.0, 1.0f,

	// z
	0.0, 0.0, 0.0, 1.0f,
	0.0, 0.0, 1.0, 1.0f,
	0.025f, 0.075f, 1.0, 1.0f,
	0.075f, 0.075f, 1.0, 1.0f,
	0.025f, 0.025f, 1.0, 1.0f,
	0.075f, 0.025f, 1.0, 1.0f
};

// Packed colour buffer for principle axes model
static float paColourArray[] = {

	// x
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	// y
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	// z
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
};


// Line list topology to render principle axes
static unsigned int paIndexArray[] = {

	0, 1, 2, 3, 4, 5,					// x
	6, 7, 8, 9, 10, 11,					// y
	12, 13, 14, 15, 15, 16, 16, 17		// z
};



PrincipleAxes::PrincipleAxes() {

	// setup shader for principle axes
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("..\\..\\Resources\\CoreStructures\\shaders\\basic_shader.vs"), string("..\\..\\Resources\\CoreStructures\\Shaders\\basic_shader.fs"), &paShader);

	// setup VAO for principle axes object
	glGenVertexArrays(1, &paVertexArrayObj);
	glBindVertexArray(paVertexArrayObj);


	// setup vbo for position attribute
	glGenBuffers(1, &paVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, paVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paPositionArray), paPositionArray, GL_STATIC_DRAW);

	// setup vertex shader attribute bindings (connecting current <position> buffer to associated 'in' variable in vertex shader)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	// setup vbo for colour attribute
	glGenBuffers(1, &paColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, paColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paColourArray), paColourArray, GL_STATIC_DRAW);

	// setup vertex shader attribute bindings (connecting current <colour> buffer to associated 'in' variable in vertex shader)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, nullptr);

	// setup ebo for principle axis (pa) index buffer
	glGenBuffers(1, &paIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(paIndexArray), paIndexArray, GL_STATIC_DRAW);

	// enable vertex buffers for principle axes rendering (vertex positions and colour buffers)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// unbind principle axes VAO
	glBindVertexArray(0);
}


PrincipleAxes::~PrincipleAxes() {

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &paVertexBuffer);
	glDeleteBuffers(1, &paColourBuffer);
	glDeleteBuffers(1, &paIndexBuffer);

	glDeleteVertexArrays(1, &paVertexArrayObj);

	glDeleteShader(paShader);
}


void PrincipleAxes::render(const glm::mat4& T) {

	static GLint mvpLocation = glGetUniformLocation(paShader, "mvpMatrix");

	glUseProgram(paShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(T));

	glBindVertexArray(paVertexArrayObj);

	glDrawElements(GL_LINES, 20, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glUseProgram(0);
}


