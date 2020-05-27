#include "NoiseImages.h"

using namespace std;

// generate an image based on Perlin noise
FloatImage *perlinNoiseImage(const Noise* P, const int w, const int h, const float scale) {

	if (P)
		return new FloatImage(w, h, [P, scale](int x, int y) -> float{return P->noise((float)x, (float)y, scale); });
	else
		return NULL;
}


// create a stochastic spectral noise function for a given frequency range [f1, f2] with amplitude a
FloatImage *snImage(const Noise* P, const int w, const int h, const float f1, const float f2, const float a) {

	if (P)
		return new FloatImage(w, h, [=](int x, int y) -> float {

		float result = 0.0f;

		for (float f = f1; f <= f2; f *= 2.0f) {
			result += P->noise((float)x, (float)y, f) * a;
		}

		return result; });

	else
		return NULL;
}


// create a turbulence image with a fractal power spectrum
FloatImage *turbulence(const Noise* P, const int w, const int h, const float f1, const float f2) {

	if (P)
		return new FloatImage(w, h, [=](int x, int y) -> float {

		float result = 0.0f;

		for (float f = f1; f <= f2; f *= 2.0f)
			result += fabs(P->noise((float)x, (float)y, f)) / f;

		return result; });

	else
		return NULL;
}


FloatImage *waveModulation(const Noise* P, const int w, const int h, const float f1, const float f2, const glm::vec2 &domainOrigin, const float domainRange) {

	float phaseShift = 10.0f;
	float amplitudeScale = 50.0f;
	float radToNoiseRatio = (float)w / domainRange;
	float dxy = domainRange / (float)w;

	if (P)
		return new FloatImage(w, h, [=](int x, int y) -> float {

		float x_ = (float)x * dxy + domainOrigin.x;
		float y_ = (float)y * dxy + domainOrigin.y;
		float nx = x_ * radToNoiseRatio;
		float ny = y_ * radToNoiseRatio;

		float result = 0.0f;

		for (float a = 0.0f, b = 0.0f, f = f1; f <= f2; f *= 2.0f) {

			a += (sinf(x_ + P->noise(nx, ny, f) * phaseShift) * (fabsf(P->noise(nx, ny, f))*amplitudeScale)) / f;
			b += (cosf(y_ + P->noise(nx, ny, f) * phaseShift) * (fabsf(P->noise(nx, ny, f))*amplitudeScale)) / f;

			result += a * b;
		}

		return result;

	});

	else
		return NULL;

}


// fractional brownian motion
FloatImage *fBM(const Noise* P, const int w, const int h, const float octaves, const float lacunarity, const float H, const float scale) {

	if (P)

		return new FloatImage(w, h, [=](int x, int y) -> float {

		glm::vec2 point = glm::vec2((float)x*scale, (float)y*scale);

		float value = 0.0f;
		int i;

		for (i = 0; i<(int)octaves; i++) {

			value += P->noise(point.x, point.y, 1.0f) * powf(lacunarity, -H * (float)i);

			point.x *= lacunarity;
			point.y *= lacunarity;
		}

		float remainder = octaves - floorf(octaves);

		if (!fabs(remainder - 0.0f) < glm::epsilon<float>())
			value += remainder * P->noise(point.x, point.y, 1.0f) * powf(lacunarity, -H * (float)i);

		return value; });

	else
		return NULL;
}


// the following functions implement hybrid multipicative/add multi-fractal functions (see Musgrave ch. 16 in Ebert et al).  These functions, like the above are based on point evaluation and do not require neighbourhoods (as in midpoint displacement for example).  This class of function is more suitable for shader implementation



// stats-by-altitude method
FloatImage *multifractal_dh1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale) {

	if (!P)
		return 0;

	// calculate static array of exponent values
	static double *exponentArray = NULL;

	if (exponentArray == NULL) {

		// create exponents array
		exponentArray = (double*)malloc(((int)octaves + 1) * sizeof(double));

		if (exponentArray) {

			double frequency = 1.0;
			for (int i = 0; i <= (int)octaves; i++, frequency *= lacunarity)
				exponentArray[i] = pow(frequency, -H);
		}
	}


	// validate existance of exponent array
	if (!exponentArray)
		return 0;

	double *data = new double[w * h];
	float *dataF = new float[w * h];

	if (!data || !dataF) {

		if (data)
			delete [] data;
		if (dataF)
			delete [] dataF;

		return 0;
	}

	double *dptr = data;

	double minValue, maxValue;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++) {

			double px = (double)x * scale;
			double py = (double)y * scale;

			double value = ((double)P->noise((float)px, (float)py, 1.0f) + offset) * exponentArray[0];

			double weight = value;

			px *= lacunarity;
			py *= lacunarity;

			int i;

			for (i = 1; i<(int)octaves; i++) {

				if (weight > 1.0)
					weight = 1.0;

				double signal = ((double)P->noise((float)px, (float)py, 1.0f) + offset) * exponentArray[i];

				value += weight * signal;

				weight *= signal;

				px *= lacunarity;
				py *= lacunarity;
			}

			double remainder = octaves - floor(octaves);

			if (abs(remainder)>0.00000001) {

				value += remainder * (double)P->noise((float)px, (float)py, 1.0f) * exponentArray[i];
			}

			*dptr = value;

			if (x == 0 && y == 0)
				minValue = maxValue = value;
			else {

				minValue = (value < minValue) ? value : minValue;
				maxValue = (value > maxValue) ? value : maxValue;
			}
		}
	}

	std::cout << minValue << ", " << maxValue << std::endl;

	// normalise
	dptr = data;

	maxValue = max(abs(minValue), abs(maxValue));

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++)
			*dptr = *dptr / maxValue;
	}


	// copy to float image
	dptr = data;
	float *fptr = dataF;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++, fptr++)
			*fptr = (float)(*dptr);
	}


	FloatImage *I = new FloatImage();

	if (I) {

		I->w = w;
		I->h = h;
		I->data = dataF;

	}
	else {

		delete [] dataF;
	}

	delete [] data;

	return I;
}


// rigid multifractal
FloatImage *multifractal_dhR1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double gain, const double scale) {

	if (!P)
		return 0;

	// calculate static array of exponent values
	static double *exponentArray = NULL;

	if (exponentArray == NULL) {

		// create exponents array
		exponentArray = (double*)malloc(((int)octaves + 1) * sizeof(double));

		if (exponentArray) {

			double frequency = 1.0;
			for (int i = 0; i <= (int)octaves; i++, frequency *= lacunarity)
				exponentArray[i] = pow(frequency, -H);
		}
	}


	// validate existance of exponent array
	if (!exponentArray)
		return 0;

	double *data = new double[w * h];
	float *dataF = new float[w * h];

	if (!data || !dataF) {

		if (data)
			delete [] data;
		if (dataF)
			delete [] dataF;

		return 0;
	}

	double *dptr = data;

	double minValue, maxValue;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++) {

			double px = (double)x * scale;
			double py = (double)y * scale;

			double signal = (double)P->noise((float)px, (float)py, 1.0f);

			if (signal < 0.0)
				signal = -signal;

			signal = offset - signal;

			signal *= signal;

			double value = signal;
			double weight = 1.0;

			int i;

			for (i = 1; i<(int)octaves; i++) {

				px *= lacunarity;
				py *= lacunarity;

				weight = signal * gain;

				if (weight > 1.0)
					weight = 1.0;
				if (weight < 0.0)
					weight = 0.0;

				signal = (double)P->noise((float)px, (float)py, 1.0f);

				if (signal < 0.0)
					signal = -signal;

				signal = offset - signal;

				signal *= signal;

				signal *= weight;

				value += signal * exponentArray[i];
			}

			*dptr = value;

			if (x == 0 && y == 0)
				minValue = maxValue = value;
			else {

				minValue = (value < minValue) ? value : minValue;
				maxValue = (value > maxValue) ? value : maxValue;
			}
		}
	}

	std::cout << minValue << ", " << maxValue << std::endl;

	// normalise
	dptr = data;

	maxValue = max(abs(minValue), abs(maxValue));

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++)
			*dptr = *dptr / maxValue;
	}


	// copy to float image
	dptr = data;
	float *fptr = dataF;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++, fptr++)
			*fptr = (float)(*dptr);
	}


	FloatImage *I = new FloatImage();

	if (I) {

		I->w = w;
		I->h = h;
		I->data = dataF;

	}
	else {

		delete [] dataF;
	}

	delete [] data;

	return I;
}



// stats-by-altitude method
FloatImage *multifractal_dsa1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale) {

	if (!P)
		return 0;

	// calculate static array of exponent values
	static double *exponentArray = NULL;

	if (exponentArray == NULL) {

		// create exponents array
		exponentArray = (double*)malloc(((int)octaves + 1) * sizeof(double));

		if (exponentArray) {

			double frequency = 1.0;
			for (int i = 0; i <= (int)octaves; i++, frequency *= lacunarity)
				exponentArray[i] = pow(frequency, -H);
		}
	}


	// validate existance of exponent array
	if (!exponentArray)
		return 0;

	double *data = new double[w * h];
	float *dataF = new float[w * h];

	if (!data || !dataF) {

		if (data)
			delete [] data;
		if (dataF)
			delete [] dataF;

		return 0;
	}

	double *dptr = data;

	double minValue, maxValue;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++) {

			double px = (double)x * scale;
			double py = (double)y * scale;

			double value = offset + (double)P->noise((float)px, (float)py, 1.0f);
			px *= lacunarity;
			py *= lacunarity;

			double increment;
			int i;

			for (i = 1; i<(int)octaves; i++) {

				double increment = (double)P->noise((float)px, (float)py, 1.0f) + offset;

				increment *= exponentArray[i];

				increment *= value;

				value += increment;

				px *= lacunarity;
				py *= lacunarity;
			}

			double remainder = octaves - floor(octaves);

			if (abs(remainder)>0.00000001) {

				increment = ((double)P->noise((float)px, (float)py, 1.0f) + offset) * exponentArray[i];
				value += remainder * increment * value;
			}

			*dptr = value;

			if (x == 0 && y == 0)
				minValue = maxValue = value;
			else {

				minValue = (value < minValue) ? value : minValue;
				maxValue = (value > maxValue) ? value : maxValue;
			}
		}
	}

	std::cout << minValue << ", " << maxValue << std::endl;

	// normalise
	dptr = data;

	maxValue = max(abs(minValue), abs(maxValue));

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++)
			*dptr = *dptr / maxValue;
	}


	// copy to float image
	dptr = data;
	float *fptr = dataF;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++, fptr++)
			*fptr = (float)(*dptr);
	}


	FloatImage *I = new FloatImage();

	if (I) {

		I->w = w;
		I->h = h;
		I->data = dataF;

	}
	else {

		delete [] dataF;
	}

	delete [] data;

	return I;
}





// the following versions of multifractal are from Ebert et al chapter 14.  This function is unstable but offers a 'pure' multi-fractal generator.  The output needs to be post-processed.  Double precision values are required since floating point values do not provide sufficient precision

FloatImage *multifractal(const Noise* P, const int w, const int h, const float octaves, const float lacunarity, const float offset, const float H, const float scale) {

	if (P)

		return new FloatImage(w, h, [=](int x, int y) -> float {

		glm::vec2 point = glm::vec2((float)x*scale, (float)y*scale);

		float value = 1.0f;
		int i;

		for (i = 0; i<(int)octaves; i++) {

			value *= (P->noise(point.x, point.y, 1.0f) + offset) * powf(lacunarity, -H * (float)i);

			point.x *= lacunarity;
			point.y *= lacunarity;
		}

		return value; });

	else
		return NULL;
}




// test multifractal with double precision arithmetic for accuracy
FloatImage *multifractal_d(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale) {

	if (!P)
		return 0;

	double *data = new double[w * h];
	float *dataF = new float[w * h];

	if (!data || !dataF) {

		if (data)
			delete [] data;
		if (dataF)
			delete [] dataF;

		return 0;
	}

	double *dptr = data;

	double minValue, maxValue;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++) {

			double px = (double)x * scale;
			double py = (double)y * scale;

			double value = 1.0f;
			int i;

			for (i = 0; i<(int)octaves; i++) {

				value *= ((double)P->noise((float)px, (float)py, 1.0f) + offset) * pow(lacunarity, -H * (double)i);

				px *= lacunarity;
				py *= lacunarity;
			}

			*dptr = value;

			if (x == 0 && y == 0)
				minValue = maxValue = value;
			else {

				minValue = (value < minValue) ? value : minValue;
				maxValue = (value > maxValue) ? value : maxValue;
			}
		}
	}

	std::cout << minValue << ", " << maxValue << std::endl;

	// normalise
	dptr = data;

	maxValue = max(abs(minValue), abs(maxValue));

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++)
			*dptr = *dptr / maxValue;
	}


	// copy to float image
	dptr = data;
	float *fptr = dataF;

	for (int y = 0; y<h; y++) {

		for (int x = 0; x<w; x++, dptr++, fptr++)
			*fptr = (float)(*dptr);
	}


	FloatImage *I = new FloatImage();

	if (I) {

		I->w = w;
		I->h = h;
		I->data = dataF;

	}
	else {

		delete [] dataF;
	}

	delete [] data;

	return I;
}
