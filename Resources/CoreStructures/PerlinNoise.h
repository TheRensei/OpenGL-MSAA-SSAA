#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H
#include "Noise.h"

class PerlinNoise : public Noise {

private:

	int							N; // domain size
	int							Nmask; // domain mask (N-1)

	int							*iTable; // self-referential index table in range [0, N)
	glm::vec2					*vTable; // ordered vector table (can be randomized if need be - Perlin's spherical vector approach adopted here)


	//
	// Private API
	//

	void getVector(const int x, const int y, glm::vec2 *p) const; // return the gradient vector from lattice point (x % N, y % N)

	float W(const float x) const; // sigmoidal weighting of gradient coefficient x


public:

	PerlinNoise(const unsigned int domainSize = 256); // constructor
	~PerlinNoise(); // destructor


	// GUNoise interface

	const int* indexTable() const;
	const glm::vec2* vectorTable() const;
	int domainSize() const;
	float noise(float x, float y, float scale) const; // return the gradient noise value at domain coordinates (x*scale, y*scale)

};
#endif 