#ifndef NOISE_IMAGES_H
#define NOISE_IMAGES_H
#include <iostream>
#include "Noise.h"
#include "FloatImage.h"


// generate an image based on Perlin noise
FloatImage *perlinNoiseImage(const Noise* P, const int w, const int h, const float scale);

// create a stochastic spectral noise function for a given frequency range [f1, f2] with amplitude a
FloatImage *snImage(const Noise* P, const int w, const int h, const float f1, const float f2, const float a);

// create a noise pattern with fractal power spectrum
FloatImage *fnImage(const Noise* P, const int w, const int h, const float f1, const float f2);

// create a turbulence image with a fractal power spectrum
FloatImage *turbulence(const Noise* P, const int w, const int h, const float f1, const float f2);

FloatImage *waveModulation(const Noise* P, const int w, const int h, const float f1, const float f2, const glm::vec2 &domainOrigin, const float domainRange);


// fractional brownian motion
FloatImage *fBM(const Noise* P, const int w, const int h, const float octaves, const float lacunarity, const float H, const float scale);


// the following functions implement hybrid multipicative/add multi-fractal functions (see Musgrave ch. 16 in Ebert et al).  These functions, like the above are based on point evaluation and do not require neighbourhoods (as in midpoint displacement for example).  This class of function is more suitable for shader implementation

// stats-by-altitude method
FloatImage *multifractal_dh1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale);

// rigid multifractal
FloatImage *multifractal_dhR1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double gain, const double scale);

// stats-by-altitude method
FloatImage *multifractal_dsa1(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale);



// the following versions of multifractal are from Ebert et al chapter 14.  This function is unstable but offers a 'pure' multi-fractal generator.  The output needs to be post-processed.  Double precision values are required since floating point values do not provide sufficient precision

FloatImage *multifractal(const Noise* P, const int w, const int h, const float octaves, const float lacunarity, const float offset, const float H, const float scale);


// test multifractal with double precision arithmetic for accuracy
FloatImage *multifractal_d(const Noise* P, const int w, const int h, const double octaves, const double lacunarity, const double offset, const double H, const double scale);

#endif