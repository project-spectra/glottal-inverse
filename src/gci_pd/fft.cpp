/* 
 * Free FFT and convolution (C++)
 * 
 * Copyright (c) 2018 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include "gci_pd.h"


using namespace arma;


static void fft_radix2(cx_vec& vec, double radius);
static void fft_bluestein(cx_vec& vec, double radius);

static void circ_conv(const cx_vec& x, const cx_vec& y, cx_vec& out, double radius);
static size_t reverseBits(size_t x, int n);


void fft_radius(cx_vec& vec, double radius) {
    int n(vec.n_elem);
    if (n == 0)
        return;
    else if ((n & (n - 1)) == 0) // is power of 2
        fft_radix2(vec, radius);
    else
        fft_bluestein(vec, radius);
}

void ifft_radius(cx_vec& vec, double radius) {
    vec = conj(vec);
    fft_radius(vec, radius);
    vec = conj(vec);
}


void fft_radix2(cx_vec& vec, double radius) {
    size_t n(vec.n_elem);
	int levels = 0;  // Compute levels = floor(log2(n))
	for (size_t temp = n; temp > 1U; temp >>= 1)
		levels++;
	if (static_cast<size_t>(1U) << levels != n)
		throw std::domain_error("FFT length is not a power of 2");
    
    // Trig table
    cx_vec expTable = radius * exp(cx_double(0., 1.) * linspace<cx_vec>(0, -M_PI * (1. - 2. / (double) n), n / 2));

    // Bit reversed addressing permutation
    for (size_t i = 0; i < n; ++i) {
        size_t j = reverseBits(i, levels);
        if (j > i)
            std::swap(vec(i), vec(j));
    }

    // Cooley-Tukey DiT radix-2 FFT
    for (size_t size = 2; size <= n; size *= 2) {
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (size_t i = 0; i < n; i += size) {
			for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
				cx_double temp = vec(j + halfsize) * expTable(k);
				vec(j + halfsize) = vec(j) - temp;
				vec(j) += temp;
			}
		}
		if (size == n)  // Prevent overflow in 'size *= 2'
			break;
	}
}

void fft_bluestein(cx_vec& vec, double radius) {
	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
	size_t n = vec.size();
	size_t m = 1;
	while (m / 2 <= n) {
		if (m > SIZE_MAX / 2)
			throw std::length_error("Vector too large");
		m *= 2;
	}
	
	// Trignometric table
	cx_vec expTable = zeros<cx_vec>(n);
	for (size_t i = 0; i < n; i++) {
		//unsigned long long temp = static_cast<unsigned long long>(i) * i;
		//temp %= static_cast<unsigned long long>(n) * 2;
		//double angle = M_PI * temp / n;
        
		// Less accurate alternative if long long is unavailable: double angle = M_PI * i * i / n;
		expTable[i] = radius * std::exp(cx_double(0, -(M_PI * i * i) / (double) n));
	}
	
	// Temporary vectors and preprocessing
    cx_vec av = zeros<cx_vec>(m);
    for (size_t i = 0; i < n; ++i)
        av(i) = vec(i) * expTable(i);

	cx_vec bv = zeros<cx_vec>(m);
    bv(0) = expTable(0);
	for (size_t i = 1; i < n; ++i)
		bv(i) = bv(m - i) = std::conj(expTable(i));
	
	// Convolution
	cx_vec cv;
	circ_conv(av, bv, cv, radius);
	
	// Postprocessing
    for (size_t i = 0; i < n; ++i)
        vec(i) = cv(i) * expTable(i);
}

void circ_conv(const cx_vec& x, const cx_vec& y, cx_vec& out, double radius) {
    size_t n = x.n_elem;
    if (n != y.n_elem) {
        throw std::length_error("Circular convolution operands must be the same size");
    }

    cx_vec xv = x;
    cx_vec yv = y;

    fft_radius(xv);
    fft_radius(yv);

    for (size_t i = 0; i < n; ++i)
        xv(i) *= yv(i);

    ifft_radius(xv);

    out = xv / (double) n;
}

size_t reverseBits(size_t x, int n) {
    size_t result = 0;
    for (int i = 0; i < n; ++i, x >>= 1)
        result = (result << 1) | (x & 1U);
    return result;
}
