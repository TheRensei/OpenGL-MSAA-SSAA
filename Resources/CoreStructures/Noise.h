#ifndef NOISE_H
#define NOISE_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Noise{
public:

	virtual const int* indexTable() const = 0;
	virtual const glm::vec2* vectorTable() const = 0;
	virtual int domainSize() const = 0;
	virtual float noise(float x, float y, float scale) const = 0;
};

#endif