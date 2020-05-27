#include "PerlinNoise.h"
#include "ctime"

using namespace std;

//
// Private API implementation
//

void PerlinNoise::getVector(const int x, const int y, glm::vec2 *p) const {

	*p = vTable[iTable[(iTable[x & Nmask] + iTable[y & Nmask]) & Nmask]];
}


// sigmoidal weighting of gradient coefficient x
float PerlinNoise::W(const float x) const {

	return (6.0f*powf(x, 5.0f)) - (15.0f*powf(x, 4.0f)) + (10.0f*powf(x, 3.0f));
}

//
// PerlinNoise public method implementation
//

PerlinNoise::PerlinNoise(const unsigned int domainSize) {

	iTable = (int *)malloc(domainSize * sizeof(int));
	vTable = (glm::vec2 *)malloc(domainSize * sizeof(glm::vec2));

	if (!iTable || !vTable) {

		// noise generator could not be created so set attributes to configure a null generator (always returns 0.0f for a noise request)

		N = 0;
		Nmask = 0;

		if (iTable) {

			free(iTable);
			iTable = NULL;
		}

		if (vTable) {

			free(vTable);
			vTable = NULL;
		}

	}
	else {

		N = domainSize;
		Nmask = N - 1;

		// seed random number generator with current system time
		srand((unsigned)time(NULL));
		rand();

		float theta = 0.0f;
		float pi2 = glm::pi<float>() * 2.0f;
		float angleStep = pi2 / (float)N;

		// initialise vector table
		for (int i = 0; i<N; i++) {

			iTable[i] = rand() % Nmask;

			vTable[i].x = cos(theta);
			vTable[i].y = sin(theta); // unit length gradient vector

			theta += angleStep;
		}
	}
}


PerlinNoise::~PerlinNoise() {

	if (iTable)
		free(iTable);

	if (vTable)
		free(vTable);
}



// GUNoise interface

const int* PerlinNoise::indexTable() const {

	return iTable;
}


const glm::vec2* PerlinNoise::vectorTable() const {

	return vTable;
}


int PerlinNoise::domainSize() const {

	return N;
}


float PerlinNoise::noise(float x, float y, float scale) const {

	// calculate noise domain coordinate
	glm::vec2	pos = glm::vec2(x*scale, y*scale);

	float x0 = floor(pos.x);
	float x1 = x0 + 1.0f;
	float y0 = floor(pos.y);
	float y1 = y0 + 1.0f;

	glm::vec2			v0, v1, v2, v3;

	getVector((int)x0, (int)y0, &v0);
	getVector((int)x0, (int)y1, &v1);
	getVector((int)x1, (int)y0, &v2);
	getVector((int)x1, (int)y1, &v3);

	glm::vec2 d0(pos.x - x0, pos.y - y0),
		d1(pos.x - x0, pos.y - y1),
		d2(pos.x - x1, pos.y - y0),
		d3(pos.x - x1, pos.y - y1);

	float h0 = d0.x * v0.x + d0.y * v0.y,
		h1 = d1.x * v1.x + d1.y * v1.y,
		h2 = d2.x * v2.x + d2.y * v2.y,
		h3 = d3.x * v3.x + d3.y * v3.y;

	float sx = W(d0.x);
	float sy = W(d0.y);

	// bi-linear interpolation (do integration at each point based on obtained gradient)
	float i0 = h0 + sx * (h2 - h0);
	float i1 = h1 + sx * (h3 - h1);
	float i2 = i0 + sy * (i1 - i0);

	return i2;
}
