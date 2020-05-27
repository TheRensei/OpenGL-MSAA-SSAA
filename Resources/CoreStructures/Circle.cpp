#include "Circle.h"
#include <iostream>

using namespace std;

Circle::Circle(int numVertices, float radius)
{
	numElements = numVertices + 2;

	// 1. setup VBO for vertex positions (bind to attribute location 0) type: vec4
	vector<glm::vec4> positionArray;

	// 2. setup VBO for vertex colour array (bind to attribute location 2) type:vec4
	vector<glm::vec4> colourArray;

	// 3. setup VBO for vertex normals (bind to attribute location 1) type: vec3
	vector<glm::vec3> normalArray;


	// Draw the centre of the circle first...
	positionArray.emplace_back(glm::vec4(0.0, 0.0, 0.0, 1.0));

	colourArray.emplace_back(glm::vec4(1.0, 1.0, 1.0, 1.0));

	normalArray.emplace_back(glm::vec3(0.0, 0.0, 1.0));

	float theta = 0.0f;
	float thetaDelta = (glm::pi<float>() * 2.0f) / float(numVertices);

	// ... then draw the points around the circumference.
	for (unsigned int i = 1; i < numElements; ++i, theta += thetaDelta)
	{
		positionArray.emplace_back(glm::vec4(cosf(theta) * radius, sinf(theta) * radius, 0.0, 1.0));

		colourArray.emplace_back(glm::vec4(cosf(theta), sinf(theta), sinf(theta)*cosf(theta), 1.0));

		normalArray.emplace_back(glm::vec3(cosf(theta), sinf(theta), 0.0));
	}

	cout << glm::to_string(normalArray[2]) << endl;

	// setup VAO for circle object
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);

	glGenBuffers(1, &meshPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, numElements * sizeof(glm::vec4), positionArray.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(0);


	glGenBuffers(1, &meshNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, numElements * sizeof(glm::vec3), normalArray.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);


	glGenBuffers(1, &meshColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshColourVBO);
	glBufferData(GL_ARRAY_BUFFER, numElements * sizeof(glm::vec4), colourArray.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

Circle::~Circle()
{

	// dispose of OpenGL resources

	// delete VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshPositionVBO);
	glDeleteBuffers(1, &meshNormalVBO);
	glDeleteBuffers(1, &meshColourVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshIndexVBO);

	// delete VAO
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &modelVAO);
}

void Circle::render() {

	glBindVertexArray(modelVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, numElements);
	glBindVertexArray(0);
}
