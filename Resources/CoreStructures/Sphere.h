#ifndef SPHERE_H
#define SPHERE_H
// CGSphere version 1.0
// Class to encapsulate a procedural sphere model stored in a VBO

/*

The attribute location assignments for each VBO in the CGSphere class are...

Vertex attribute (VBO)		packet attribute location
----------------------		-------------------------
position					0
(diffuse) colour			1
normal						2
texture coord 0				3

*/

#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// Model a sphere procedurally. The z coordinates are determined by the handedness specified in the constructor parameter.  The z coordinate is inverted and face ordering reversed for the right-handed system (OpenGL).  The default is the left-handed system (DirectX)

enum SphereHandedness { CG_LEFTHANDED, CG_RIGHTHANDED };

class Sphere{

private:

	GLuint			sphereVAO;

	GLuint			meshPositionVBO;
	GLuint			meshColourVBO;
	GLuint			meshNormalVBO;
	GLuint			meshTexCoordVBO;
	GLuint			meshIndexVBO;

	GLuint			numElements;

public:

	Sphere();
	Sphere(int n, int m, float radius, const glm::vec4& vertexColour, const SphereHandedness orientation = CG_LEFTHANDED);  // n = slices, m = stacks
	~Sphere();

	void render(); // encapsulate rendering with VBOs
};

#endif