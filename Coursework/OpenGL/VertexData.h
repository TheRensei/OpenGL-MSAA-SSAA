// Per-vertex position vectors
static float cubeVertices[32] =
{
	-1.0f, 1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f, 1.0f
};

// Per-vertex colours (RGBA) floating point values
static float cubeColours[32] =
{
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};

// 12 faces each with 3 vertices (each face forms a triangle) (36 indices total)
static unsigned short cubeVertexIndices[36] =
{
	0, 1, 2, // top (+y)
	0, 2, 3,
	4, 7, 5, // bottom (-y)
	5, 7, 6,
	0, 4, 1, // -x
	1, 4, 5,
	2, 7, 3, // +x
	2, 6, 7,
	0, 3, 4, // -z
	3, 7, 4,
	1, 5, 2, // +z
	2, 5, 6
};

// Per-vertex position vectors
static float pyramidVertices[20] =
{
	0.0f, 2.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, -1.0f, 1.0f,
	-1.0f, 0.0f, -1.0f, 1.0f,
	-1.0f, 0.0f, 1.0f, 1.0f
};

// Per-vertex colours (RGBA) floating point values
static float		pyramidColours[20] =
{
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.0f, 0.5f, 1.0f,
	0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.0f, 1.0f
};

// 5 faces each with 3 vertices (each face forms a triangle)
static unsigned short		pyramidVertexIndices[18] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 1,
	3, 2, 4,
	1, 4, 2
};
