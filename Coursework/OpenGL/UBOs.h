#ifndef UBOS
#define UBOS
#include <glm/gtc/type_ptr.hpp>

struct DirLight 
{
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

struct PointLight
{
	glm::vec4 position;

	float constant;
	float linear;
	float quadratic;
	float PADDING = 0.0f;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

struct Transforms
{
	glm::mat4 model;
	glm::mat4 viewProjection;
	glm::mat4 invTranspose;
	glm::vec4 viewPosition;
};

#endif
