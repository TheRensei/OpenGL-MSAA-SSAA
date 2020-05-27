#ifndef FLOAT_IMAGE_H
#define FLOAT_IMAGE_H
#include <functional>
#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/epsilon.hpp>

typedef enum {

	gu_floatimage_periodic, // straigforward 1N periodicity of image signal
	gu_floatimage_even_w, // 2N even extension around i = w-1 where 0 ≤ i < w {...dcbabcd...}
	gu_floatimage_even_wh, // 2N even extension around i = w+1/2 {...dcbaabcd...} (default)

} gu_floatimage_extension_type;


class FloatImage{

public:

	int			w, h;
	float		*data;


public:

	FloatImage();
	FloatImage(int imageWidth, int imageHeight); // create a GUFloatImage of size (imageWidth, imageHeight) with all elements initialised to 0.0
	FloatImage(int imageWidth, int imageHeight, std::function<float(int, int)> fn);
	FloatImage(FloatImage *I); // copy constructor

	~FloatImage();

	bool isValid();

	int imageWidth();
	int imageHeight();


	FloatImage *subtractImage(FloatImage *B); // return a new GUFloatImage *this - B.  Return a NULL image if A.w != B.w || A.h != B.h or either *this or B are NULL images (ie. w=h=data=0)

	void mapToRange(float mapMin, float mapMax); // directly map image data from the current range given by getImageRange() to the range defined by [mapMin, mapMax].  The mapped values depend on the range extent so the mapped location of 0.0 is determined by the mapping to the new range.  Therefore assumptions should not be made about the location 0.0 maps to.  If this is important then normalise() should be used instead followed by a suitable mapping closure executed using eval().  This replaces the depricated mapToUnitRange used in older versions of the class

	void normalise(); // normalise image values into the range [-1, 1].  It is important the relationship to I(x,y)=0.0 is maintained so the following conditions apply.  If the image lies in the [-1, 1] range then nothing is changed.  If the image is homogeneous and every pixel lies outside the range [-1, 1] then the resulting image I(x,y) = {0.0 : ∀(x, y)} is returned.  Otherwise we normalise according the largest absolute extent.  If all I(x,y) ≥ 0.0 ∀(x, y) then the resulting image will have pixels in the range [0, 1], if all I(x,y) ≤ 0.0 ∀(x, y) then the resulting image will have pixels in the range [-1, 0], otherwise, if min(I(x, y))<0.0 and max(I(x, y))>0.0 then the resulting image is normalied to [-1, 1] with the largest absolute is used as the normalisation coefficient

	void normalise(float rangeExtent); // overloaded version of normalise that divides each image value by rangeExtent.  If rangeExtent = 0.0 as determined by equalf() then a homogeneous image I(x,y) = {0.0 : ∀(x, y)} is returned

	void eval(std::function<float(int, int, float)> fn); // process each pixel in turn with fn

	GLuint createOpenGLTexture(); // overloaded version of createOpenGLTexture that calls the designated version of createOpenGLTexture with default parameters.  The texture is setup with GL_LUMINANCE internal format

	GLuint createOpenGLTexture(GLenum internalFormat); // overloaded version of createOpenGLTexture that calls the designated version of createOpenGLTexture with default filter parameters but specifies the internal texture format

	GLuint createOpenGLTexture(GLenum internalFormat, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT); // create OpenGL texture using the encapsulated image data.  The texture format is determined by the internalFormat parameter


	FloatImage *subImage(int x, int y, int subWidth, int subHeight);

	float &operator()(int x, int y);

	void getImageRange(float *minValue, float *maxValue); // return the minimum and maximum values within the image

	FloatImage *upSample(); // use quadrilateral interpolation subdivision to upsample a given image of size (w x h) to size (2w-1 x 2h-1).  This is important since we subdivide between points (pixels) and do not extrapolate outside the defined extent of the image

	FloatImage *upSample2(); // use quadrilateral interpolation subdivision to upsample a given image of size (w x h) to size (2w x 2h).  Unlike the upSample method, upSample2 ensures the size of the upsampled image is 2x the width and the height of the source image by extrapolating the right and bottom edges of the resulting image (see notes)

	FloatImage *downSample2(); // downsample *this by 1/2 - apply simple averaging scheme to each down-sampled block

	FloatImage *extendImage(int N, gu_floatimage_extension_type extType = gu_floatimage_even_wh); // return a new GUFloatImage that contains the extended image according to the extension function extType and extension size N

	FloatImage *extendHorizontally(int N, gu_floatimage_extension_type extType = gu_floatimage_even_wh);

	FloatImage *extendVertically(int N, gu_floatimage_extension_type extType = gu_floatimage_even_wh);

};
#endif