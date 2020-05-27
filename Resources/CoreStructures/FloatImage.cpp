// FloatImage.cpp

#include "FloatImage.h"
#include <ostream>
#include <iostream>

using namespace std;

FloatImage::FloatImage() {

	w = 0;
	h = 0;
	data = NULL;
}


FloatImage::FloatImage(int imageWidth, int imageHeight) {

	data = new float[imageWidth * imageHeight];

	if (!data) {

		w = 0;
		h = 0;

	}
	else {

		w = imageWidth;
		h = imageHeight;
	}
}


FloatImage::FloatImage(int imageWidth, int imageHeight, function<float(int, int)> fn) {

	data = new float[imageWidth * imageHeight];

	if (!data) {

		w = 0;
		h = 0;

	}
	else {

		w = imageWidth;
		h = imageHeight;

		float *ptr = data;

		for (int y = 0; y<h; y++) {

			for (int x = 0; x<w; x++, ptr++) {

				*ptr = fn(x, y);
			}
		}
	}
}


// copy constructor
FloatImage::FloatImage(FloatImage *I) {

	data = new float[I->w * I->h];

	if (data) {

		w = I->w;
		h = I->h;

		memcpy(data, I->data, w*h*sizeof(float));

	}
	else {

		w = h = 0;
	}
}


FloatImage::~FloatImage() {

	if (data)
		free(data);
}


bool FloatImage::isValid() {

	return (data != NULL);
}


int FloatImage::imageWidth() {

	return w;
}


int FloatImage::imageHeight() {

	return h;
}



// return a new FloatImage *this - B.  Return a NULL image if A.w != B.w || A.h != B.h or either *this or B are NULL images (ie. w=h=data=0)
FloatImage *FloatImage::subtractImage(FloatImage *B) {

	FloatImage *newImage = NULL;

	if (data && B->data && w == B->w && h == B->h) {

		newImage = new FloatImage(w, h);

		if (newImage) {

			float *aPtr = data;
			float *bPtr = B->data;
			float *rPtr = newImage->data;

			for (int y = 0; y<h; y++) {

				for (int x = 0; x<w; x++, aPtr++, bPtr++, rPtr++)
					*rPtr = (*aPtr) - (*bPtr);
			}
		}
	}

	return newImage;
}


// directly map image data from the current range given by getImageRange() to the range defined by [mapMin, mapMax].  The mapped values depend on the range extent so the mapped location of 0.0 is determined by the mapping to the new range.  Therefore assumptions should not be made about the location 0.0 maps to.  If this is important then normalise() should be used instead followed by a suitable mapping closure executed using eval()
void FloatImage::mapToRange(float mapMin, float mapMax) {

	float min, max;

	getImageRange(&min, &max);

	float range = max - min;
	float mapRange = mapMax - mapMin;

	eval([min, range, mapRange, mapMin](int x, int y, float value)->float{return ((value - min) / range) * mapRange + mapMin; });


	//eval(^(NSInteger x, NSInteger y, gu_float value) { return ((value - min) / range) * mapRange + mapMin; });
}


// normalise image values into the range [-1, 1].  It is important the relationship to I(x,y)=0.0 is maintained so the following conditions apply.  If the image lies in the [-1, 1] range then nothing is changed.  If the image is homogeneous and every pixel lies outside the range [-1, 1] then the resulting image I(x,y) = {0.0 : ∀(x, y)} is returned.  Otherwise we normalise according the largest absolute extent.  If all I(x,y) ≥ 0.0 ∀(x, y) then the resulting image will have pixels in the range [0, 1], if all I(x,y) ≤ 0.0 ∀(x, y) then the resulting image will have pixels in the range [-1, 0], otherwise, if min(I(x, y))<0.0 and max(I(x, y))>0.0 then the resulting image is normalied to [-1, 1] with the largest extent is used as the normalisation coefficient

void FloatImage::normalise() {

	if (!data)
		return;

	float minValue, maxValue;

	getImageRange(&minValue, &maxValue);

	// do nothing if the image already lies in the [-1, +1] range
	if (minValue >= -1.0f && maxValue <= 1.0f)
		return;

	// normalise so I(x,y) = {0.0 : ∀(x, y)} if the image is homogeneous
	else if (fabs(minValue - maxValue) < glm::epsilon<float>())
		normalise(0.0f);

	else
		normalise(max(fabsf(maxValue), fabsf(minValue)));
}


// overloaded version of normalise that divides each image value by rangeExtent.  If rangeExtent = 0.0 as determined by equalf() then a homogeneous image I(x,y) = {0.0 : ∀(x, y)} is returned
void FloatImage::normalise(float rangeExtent) {

	if (fabs(rangeExtent - 0.0f) < glm::epsilon<float>())
		// return homogeneous image if rangeExtent = 0.0.  This avoids division by 0 otherwise

		eval([](int x, int y, float value)->float{return 0.0f; });
	//eval(^(NSInteger x, NSInteger y, gu_float value) { return 0.0f; });
	else
		// normalise according to image range

		eval([rangeExtent](int x, int y, float value)->float{return value / rangeExtent; });

	//eval(^(NSInteger x, NSInteger y, gu_float value) { return value / rangeExtent; });
}


// process each pixel in turn with fn
void FloatImage::eval(function<float(int, int, float)> fn) {

	if (!data)
		return;

	float *ptr = data;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, ptr++) {

			*ptr = fn(x, y, *ptr);
		}
	}
}


// overloaded version of createOpenGLTexture that calls the designated version of createOpenGLTexture with default parameters.  The texture is setup with GL_LUMINANCE internal format
GLuint FloatImage::createOpenGLTexture() {

	return createOpenGLTexture(GL_R32F);
}


// overloaded version of createOpenGLTexture that calls the designated version of createOpenGLTexture with default filter parameters but specifies the internal texture format
GLuint FloatImage::createOpenGLTexture(GLenum internalFormat) {

	return createOpenGLTexture(internalFormat, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
}


// create OpenGL texture using the encapsulated image data.  The texture format is determined by the internalFormat parameter
GLuint FloatImage::createOpenGLTexture(GLenum internalFormat, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT) {

	GLuint t = 0;

	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, GL_RED, GL_FLOAT, data);

	return t;
}


FloatImage *FloatImage::subImage(int x, int y, int subWidth, int subHeight) {

	if (!data)
		return NULL;

	x = min<int>(max<int>(x, 0), w - 1);
	y = min<int>(max<int>(y, 0), h - 1);

	if (x + subWidth>w)
		subWidth = w - x;
	if (y + subHeight>h)
		subHeight = h - y;

	FloatImage *newImage = new FloatImage(subWidth, subHeight);

	if (newImage) {

		if (newImage->data) {

			for (int i = 0; i<subHeight; i++)
				memcpy(newImage->data + (subWidth*i), data + ((y + i)*w) + x, subWidth * sizeof(float));

		}
		else {

			delete newImage;
			newImage = nullptr;
		}
	}

	return newImage;
}


float &FloatImage::operator()(int x, int y) {

	return *(data + (w*y) + x);
}


void FloatImage::getImageRange(float *minValue, float *maxValue) {

	if (!data) {

		*minValue = 0.0f;
		*maxValue = 0.0f;

	}
	else {

		float *ptr = data;

		*minValue = *ptr;
		*maxValue = *ptr;

		for (int y = 0; y<h; y++) {

			for (int x = 0; x<w; x++, ptr++) {

				if (*ptr < *minValue)
					*minValue = *ptr;

				if (*ptr > *maxValue)
					*maxValue = *ptr;
			}
		}
	}
}



// use quadrilateral interpolation subdivision to upsample a given image of size (w x h) to size (2w-1 x 2h-1).  This is important since we subdivide between points (pixels) and do not extrapolate outside the defined extent of the image.  If the image is a single pixel in dimension simply return a copy of the image without subdivision
FloatImage *FloatImage::upSample() {

	FloatImage *U = NULL;

	if (w == 1 && h == 1) {

		U = new FloatImage(this);

	}
	else {

		int uW = (w << 1) - 1;
		int uH = (h << 1) - 1;

		U = new FloatImage(uW, uH);

		if (U) {

			float *dptr = data;
			float *uptr = 0;

			const float _w = 1.0f; // subdivision scale coefficient (omega)

			float c1 = (8.0f - _w) / 16.0f;
			float c2 = (8.0f + 2.0f*_w) / 16.0f;
			float c3 = _w / 16.0f;
			float c4 = (8.0f + _w) / 16.0f;

			// pass 1 - create per-point and per-edge subdivided points
			for (int j = 0; j<h; j++) {

				for (int i = 0; i<w; i++, dptr++) {

					uptr = U->data + ((j << 1)*uW) + (i << 1);

					// 1.1 copy point P(i,j) to P(2i,2j) in U
					*uptr = *dptr;

					// 1.2 derive P(2i+1, 2j) using the 4-point subdivision rule
					if (i<w - 1) {

						if (w == 2) { // special case where i=0=w-2 so we need to extrpolate P(-1, j) and P(2, j).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes)

							*(uptr + 1) = (*dptr + *(dptr + 1)) / 2.0f;

						}
						else {

							if (i == 0) { // apply k5A - extrapolate P(i-1, j) = 2P(0, j) - P(1, j)

								*(uptr + 1) = *dptr*c1 + *(dptr + 1)*c2 - *(dptr + 2)*c3;

							}
							else if (i == w - 2) { // apply k5B - extrapolate P(i+2, j) = 2P(i+1, j) - P(i, j)

								*(uptr + 1) = *dptr*c2 + *(dptr + 1)*c1 - *(dptr - 1)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*(uptr + 1) = (*dptr + *(dptr + 1))*c4 - (*(dptr - 1) + *(dptr + 2))*c3;
							}
						}
					}

					// 1.3 derive P(2i, 2j+1) using the 4-point subdivision rule
					if (j<h - 1) {

						if (h == 2) { // special case where j=0=h-2 so we need to extrapolate P(i, -1) and P(i, 2).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes)

							*(uptr + uW) = (*dptr + *(dptr + w)) / 2.0f;

						}
						else {

							if (j == 0) { // apply k5A - extrapolate P(i, j-1) = 2P(i, 0) - P(i, 1)

								*(uptr + uW) = *dptr*c1 + *(dptr + w)*c2 - *(dptr + (w << 1))*c3;

							}
							else if (j == h - 2) { // apply k5B - extrapolate P(i, j+2) = 2P(i, j+1) - P(i, j)

								*(uptr + uW) = *dptr*c2 + *(dptr + w)*c1 - *(dptr - w)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*(uptr + uW) = (*dptr + *(dptr + w))*c4 - (*(dptr - w) + *(dptr + (w << 1)))*c3;
							}
						}
					}
				}
			}

			// pass 2 - create per-"face" subdivided points
			for (int j = 0; j<h; j++) {

				for (int i = 0; i<w; i++, dptr++) {

					// derive P(2i+1, 2j+1) - face point rule
					if (i<w - 1 && j<h - 1) {

						uptr = U->data + (((j << 1) + 1)*uW) + (i << 1) + 1;

						// given tensor product properties can use horizontal or vertical 4-point rule applied to edge points calculated in pass 1.  Here we apply the 4-point horizontal subdivision mask

						if (w == 2) { // special case where i=0=w-2 so we need to extrpolate P(-1, j) and P(2, j).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes).  This results in bilinear interpolation of initial vertex (pixel) and edge points calculated above

							*uptr = (*(uptr - 1) + *(uptr + 1)) / 2.0f;

						}
						else {

							if (i == 0) { // apply k5A - extrapolate P(2i-2, 2j+1) = 2P(2i, 2j+1) - P(2i+2, 2j+1)

								*uptr = *(uptr - 1)*c1 + *(uptr + 1)*c2 - *(uptr + 3)*c3;

							}
							else if (i == w - 2) { // apply k5B - extrapolate P(2i+4, 2j+1) = 2P(2i+2, 2j+1) - P(2i, 2j+1)

								*uptr = *(uptr + 1)*c1 + *(uptr - 1)*c2 - *(uptr - 3)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*uptr = (*(uptr - 1) + *(uptr + 1))*c4 - (*(uptr - 3) + *(uptr + 3))*c3;
							}
						}


					}
				}
			}

		}
	}

	return U;
}


// use quadrilateral interpolation subdivision to upsample a given image of size (w x h) to size (2w x 2h).  Unlike the upSample method, upSample2 ensures the size of the upsampled image is 2x the width and the height of the source image by extrapolating the right and bottom edges of the resulting image (see notes).  For 1D images (where width or height = 1.0) a 1D image is also returned and extrapolation is performed only along the axis a where |a| > 1.0
FloatImage *FloatImage::upSample2() {

	FloatImage *U = NULL;

	if (w == 1 && h == 1) {

		U = new FloatImage(this);

	}
	else {

		int uW = (w == 1) ? w : w << 1; // 4 ≤ uW
		int uH = (h == 1) ? h : h << 1; // 4 ≤ uH

		U = new FloatImage(uW, uH);

		if (U) {

			float *dptr = data;
			float *uptr = 0;

			const float _w = 1.0f; // subdivision scale coefficient (omega)

			float c1 = (8.0f - _w) / 16.0f;
			float c2 = (8.0f + 2.0f*_w) / 16.0f;
			float c3 = _w / 16.0f;
			float c4 = (8.0f + _w) / 16.0f;

			// pass 1 - create per-point and per-edge subdivided points
			for (int j = 0; j<h; j++) {

				for (int i = 0; i<w; i++, dptr++) {

					uptr = U->data + ((j << 1)*uW) + (i << 1);

					// 1.1 copy point P(i,j) to P(2i,2j) in U
					*uptr = *dptr;

					// 1.2 derive P(2i+1, 2j) using the 4-point subdivision rule
					if (i<w - 1) {

						if (w == 2) { // special case where i=0=w-2 so we need to extrpolate P(-1, j) and P(2, j).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes)

							*(uptr + 1) = (*dptr + *(dptr + 1)) / 2.0f;

						}
						else {

							if (i == 0) { // apply k5A - extrapolate P(i-1, j) = 2P(0, j) - P(1, j)

								*(uptr + 1) = *dptr*c1 + *(dptr + 1)*c2 - *(dptr + 2)*c3;

							}
							else if (i == w - 2) { // apply k5B - extrapolate P(i+2, j) = 2P(i+1, j) - P(i, j)

								*(uptr + 1) = *dptr*c2 + *(dptr + 1)*c1 - *(dptr - 1)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*(uptr + 1) = (*dptr + *(dptr + 1))*c4 - (*(dptr - 1) + *(dptr + 2))*c3;
							}
						}
					}

					// 1.3 derive P(2i, 2j+1) using the 4-point subdivision rule
					if (j<h - 1) {

						if (h == 2) { // special case where j=0=h-2 so we need to extrapolate P(i, -1) and P(i, 2).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes)

							*(uptr + uW) = (*dptr + *(dptr + w)) / 2.0f;

						}
						else {

							if (j == 0) { // apply k5A - extrapolate P(i, j-1) = 2P(i, 0) - P(i, 1)

								*(uptr + uW) = *dptr*c1 + *(dptr + w)*c2 - *(dptr + (w << 1))*c3;

							}
							else if (j == h - 2) { // apply k5B - extrapolate P(i, j+2) = 2P(i, j+1) - P(i, j)

								*(uptr + uW) = *dptr*c2 + *(dptr + w)*c1 - *(dptr - w)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*(uptr + uW) = (*dptr + *(dptr + w))*c4 - (*(dptr - w) + *(dptr + (w << 1)))*c3;
							}
						}
					}
				}
			}

			// pass 2 - create per-"face" subdivided points
			for (int j = 0; j<h; j++) {

				for (int i = 0; i<w; i++, dptr++) {

					// derive P(2i+1, 2j+1) - face point rule
					if (i<w - 1 && j<h - 1) {

						uptr = U->data + (((j << 1) + 1)*uW) + (i << 1) + 1;

						// given tensor product properties can use horizontal or vertical 4-point rule applied to edge points calculated in pass 1.  Here we apply the 4-point horizontal subdivision mask

						if (w == 2) { // special case where i=0=w-2 so we need to extrpolate P(-1, j) and P(2, j).  This results in a linear model so simply average P0 and P1.  This complies with 4-point subdivision rules (see proof in notes).  This results in bilinear interpolation of initial vertex (pixel) and edge points calculated above

							*uptr = (*(uptr - 1) + *(uptr + 1)) / 2.0f;

						}
						else {

							if (i == 0) { // apply k5A - extrapolate P(2i-2, 2j+1) = 2P(2i, 2j+1) - P(2i+2, 2j+1)

								*uptr = *(uptr - 1)*c1 + *(uptr + 1)*c2 - *(uptr + 3)*c3;

							}
							else if (i == w - 2) { // apply k5B - extrapolate P(2i+4, 2j+1) = 2P(2i+2, 2j+1) - P(2i, 2j+1)

								*uptr = *(uptr + 1)*c1 + *(uptr - 1)*c2 - *(uptr - 3)*c3;

							}
							else { // apply k2 - subdivide interior edge

								*uptr = (*(uptr - 1) + *(uptr + 1))*c4 - (*(uptr - 3) + *(uptr + 3))*c3;
							}
						}


					}
				}
			}


			// pass 3 - extrpolate right and bottom edges

			// 3.1 extrapolate right edge if necessary
			if (uW > 1) { // from above inequality 4 ≤ uW we can assume relevant points present for extrapolation

				uptr = U->data + uW - 1;

				int j = 0; // row index

				// if uH = 1 then we only extrapolate a single point since image represents a row vector.  Otherwise extrapolate right edge exluding the bottom pixel (calculate this on bottom-edge extrapolation - see notes on equivalence)

				do {

					*uptr = 2.0f * *(uptr - 1) - *(uptr - 2);
					uptr += uW;
					j++;

				} while (j<uH - 1);
			}

			// 3.2 extrapolate bottom edge if necessary
			if (uH > 1) { // from above inequality 4 ≤ uH we can assume relevant points present for extrapolation

				uptr = U->data + ((uH - 1) * uW);

				int i = 0; // column index

				// if uW = 1 then we only extrapolate a single point since image represents a column vector.  Otherwise extrapolate bottom edge including the right-most pixel since relevant points needed for extrapolation of this point are calculated in step 3.1 above (see notes)

				do {

					*uptr = 2.0f * *(uptr - uW) - *(uptr - (uW << 1));
					uptr++;
					i++;

				} while (i<uW);
			}
		}
	}

	return U;
}



// downsample *this by 1/2 - apply simple averaging scheme to each down-sampled block
FloatImage *FloatImage::downSample2() {

	if (!data)
		return NULL;

	FloatImage *ds = new FloatImage(w >> 1, h >> 1);

	if (!ds)
		return NULL;

	if (!ds->data) {

		delete ds;

		return NULL;
	}

	float *fptr = data; // current resolution data
	float *cptr = ds->data; // destination / down-sampled data

	for (int y = 0; y<ds->h; y++, fptr += w) {

		for (int x = 0; x<ds->w; x++, cptr++, fptr += 2) {

			*cptr = (*fptr + *(fptr + 1) + *(fptr + w) + *(fptr + w + 1)) / 4.0f;
		}
	}

	return ds;
}


FloatImage *FloatImage::extendImage(int N, gu_floatimage_extension_type extType) {

	// first determine initial image has sufficient extent for required extension type
	if (((extType == gu_floatimage_even_w /*|| extType==gu_floatimage_odd_w*/) && (w < N + 1 || h < N + 1)) ||
		(w < N || h < N))
		return NULL;

	// create extended image
	int w_ = w + (N << 1);
	int h_ = h + (N << 1);

	FloatImage *I = new FloatImage(w_, h_);

	if (I) {

		// copy source image into new image
		float *srcPtr = data;
		float *dstPtr = I->data + w_*N + N;

		for (int i = 0; i<h; i++, srcPtr += w, dstPtr += w_)
			memcpy(dstPtr, srcPtr, w * sizeof(float));


		// extend
		switch (extType) {

		case gu_floatimage_periodic:

			// copy top N rows
			srcPtr = data + w*(h - N);
			dstPtr = I->data + N;

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr += w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// copy bottom N rows
			srcPtr = data;
			dstPtr = I->data + w_*(h_ - N) + N;

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr += w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// copy side edges
			srcPtr = I->data + w;
			dstPtr = I->data;

			for (int i = 0; i<h_; i++, srcPtr += w_, dstPtr += w_) {

				memcpy(dstPtr, srcPtr, N * sizeof(float));
				memcpy(dstPtr + w + N, srcPtr - (w - N), N * sizeof(float));
			}

			break;


		case gu_floatimage_even_w:

			// copy top N rows
			srcPtr = data + w;
			dstPtr = I->data + (N - 1)*w_ + N;

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr -= w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// copy bottom N rows
			srcPtr = data + w*(h - 2);
			dstPtr = I->data + w_*(h_ - N) + N;

			for (int i = 0; i<N; i++, srcPtr -= w, dstPtr += w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));


			// copy side edges

			srcPtr = I->data + N + 1;
			dstPtr = I->data + N - 1;

			for (int i = 0; i<h_; i++, srcPtr += w_, dstPtr += w_) {

				float *sp = srcPtr;
				float *dp = dstPtr;

				for (int j = 0; j<N; j++, sp++, dp--)
					*dp = *sp;

				sp = srcPtr + w - 3;
				dp = dstPtr + w + 1;

				for (int j = 0; j<N; j++, sp--, dp++)
					*dp = *sp;
			}

			break;


		case gu_floatimage_even_wh:

			// copy top N rows
			srcPtr = data;
			dstPtr = I->data + (N - 1)*w_ + N;

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr -= w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// copy bottom N rows
			srcPtr = data + w*(h - 1);
			dstPtr = I->data + w_*(h_ - N) + N;

			for (int i = 0; i<N; i++, srcPtr -= w, dstPtr += w_)
				memcpy(dstPtr, srcPtr, w * sizeof(float));


			// copy side edges

			srcPtr = I->data + N;
			dstPtr = I->data + N - 1;

			for (int i = 0; i<h_; i++, srcPtr += w_, dstPtr += w_) {

				float *sp = srcPtr;
				float *dp = dstPtr;

				for (int j = 0; j<N; j++, sp++, dp--)
					*dp = *sp;

				sp = srcPtr + w - 1;
				dp = dstPtr + w + 1;

				for (int j = 0; j<N; j++, sp--, dp++)
					*dp = *sp;
			}

			break;


			// Note: each column / row is extended independently and the perpendicular relationships between pixel values are not retained in the (odd) extended region.  Therefore striping artefacts occur as each row / column is independent.  This model is suitable for 1D functions, but direct implementation of rotational symmetry of 1D functions to 2D images does not work well!
			/*
			case gu_floatimage_odd_w:

			// copy top N rows
			for (NSInteger i=1;i<=N;i++) {

			srcPtr = data + w*i;
			dstPtr = I->data + w_*(N-i) + N;

			gu_float *pivotPtr = data;

			for (NSInteger j=0;j<w;j++, srcPtr++, dstPtr++, pivotPtr++)
			*dstPtr = (*pivotPtr) + ((*pivotPtr) - (*srcPtr));
			}

			// copy bottom N rows
			for (NSInteger i=0;i<N;i++) {

			srcPtr = data + w*(h-2-i);
			dstPtr = I->data + w_*(h_-N+i) + N;

			gu_float *pivotPtr = data + w*(h-1);

			for (NSInteger j=0;j<w;j++, srcPtr++, dstPtr++, pivotPtr++)
			*dstPtr = (*pivotPtr) + ((*pivotPtr) - (*srcPtr));
			}

			// copy side edges
			for (NSInteger i=0;i<h_;i++) {

			srcPtr = I->data + (w_*i) + N + 1;
			dstPtr = I->data + (w_*i) + N - 1;

			gu_float *pivotPtr = I->data + (w_*i) + N;

			for (NSInteger j=0;j<N;j++, srcPtr++, dstPtr--)
			*dstPtr = (*pivotPtr) + ((*pivotPtr) - (*srcPtr));
			}

			for (NSInteger i=0;i<h_;i++) {

			srcPtr = I->data + (w_*i) + w + N - 2;
			dstPtr = I->data + (w_*i) + w + N;

			gu_float *pivotPtr = I->data + (w_*i) + w + N - 1;

			for (NSInteger j=0;j<N;j++, srcPtr--, dstPtr++)
			*dstPtr = (*pivotPtr) + ((*pivotPtr) - (*srcPtr));
			}

			break;
			*/

		}
	}

	return I;
}



FloatImage *FloatImage::extendHorizontally(int N, gu_floatimage_extension_type extType) {

	// first determine initial image has sufficient extent for required extension type
	if (((extType == gu_floatimage_even_w /*|| extType==gu_floatimage_odd_w*/) && (w < N + 1)) || w < N)
		return NULL;

	// create (horizontally) extended image
	int w_ = w + (N << 1);

	FloatImage *I = new FloatImage(w_, h);

	if (I) {

		// copy source image into new image
		float *srcPtr = data;
		float *dstPtr = I->data + N;

		for (int i = 0; i<h; i++, srcPtr += w, dstPtr += w_)
			memcpy(dstPtr, srcPtr, w * sizeof(float));


		// extend
		switch (extType) {

		case gu_floatimage_periodic:

			srcPtr = I->data + w;
			dstPtr = I->data;

			for (int i = 0; i<h; i++, srcPtr += w_, dstPtr += w_) {

				memcpy(dstPtr, srcPtr, N * sizeof(float));
				memcpy(dstPtr + w + N, srcPtr - (w - N), N * sizeof(float));
			}

			break;


		case gu_floatimage_even_w:

			srcPtr = I->data + N + 1;
			dstPtr = I->data + N - 1;

			for (int i = 0; i<h; i++, srcPtr += w_, dstPtr += w_) {

				float *sp = srcPtr;
				float *dp = dstPtr;

				for (int j = 0; j<N; j++, sp++, dp--)
					*dp = *sp;

				sp = srcPtr + w - 3;
				dp = dstPtr + w + 1;

				for (int j = 0; j<N; j++, sp--, dp++)
					*dp = *sp;
			}

			break;


		case gu_floatimage_even_wh:

			srcPtr = I->data + N;
			dstPtr = I->data + N - 1;

			for (int i = 0; i<h; i++, srcPtr += w_, dstPtr += w_) {

				float *sp = srcPtr;
				float *dp = dstPtr;

				for (int j = 0; j<N; j++, sp++, dp--)
					*dp = *sp;

				sp = srcPtr + w - 1;
				dp = dstPtr + w + 1;

				for (int j = 0; j<N; j++, sp--, dp++)
					*dp = *sp;
			}

			break;
		}
	}

	return I;
}

FloatImage *FloatImage::extendVertically(int N, gu_floatimage_extension_type extType) {

	// first determine initial image has sufficient extent for required extension type
	if (((extType == gu_floatimage_even_w /*|| extType==gu_floatimage_odd_w*/) && (h < N + 1)) || h < N)
		return NULL;

	// create extended image
	int h_ = h + (N << 1);

	FloatImage *I = new FloatImage(w, h_);

	if (I) {

		// copy source image into new image
		float *srcPtr = data;
		float *dstPtr = I->data + w*N;
		memcpy(dstPtr, srcPtr, w * h * sizeof(float));


		// extend
		switch (extType) {

		case gu_floatimage_periodic:

			// copy bottom N rows to top
			srcPtr = data + w*(h - N);
			dstPtr = I->data;
			memcpy(dstPtr, srcPtr, w * N * sizeof(float));

			// copy top N rows to bottom
			srcPtr = data;
			dstPtr = I->data + w*(h_ - N);
			memcpy(dstPtr, srcPtr, w * N * sizeof(float));

			break;


		case gu_floatimage_even_w:

			// reflect top N rows
			srcPtr = data + w;
			dstPtr = I->data + w*(N - 1);

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr -= w)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// reflect bottom N rows
			srcPtr = data + w*(h - 2);
			dstPtr = I->data + w*(h_ - N);

			for (int i = 0; i<N; i++, srcPtr -= w, dstPtr += w)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			break;


		case gu_floatimage_even_wh:

			// reflect top N rows
			srcPtr = data;
			dstPtr = I->data + w*(N - 1);

			for (int i = 0; i<N; i++, srcPtr += w, dstPtr -= w)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			// reflect bottom N rows
			srcPtr = data + w*(h - 1);
			dstPtr = I->data + w*(h_ - N);

			for (int i = 0; i<N; i++, srcPtr -= w, dstPtr += w)
				memcpy(dstPtr, srcPtr, w * sizeof(float));

			break;

		}
	}

	return I;
}
