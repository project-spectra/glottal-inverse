#ifndef FILTER_H
#define FILTER_H


#include <valarray>

using valarray = std::valarray<double>;


void filter_fir(const valarray& b, const valarray& x, valarray& res);

void filter_iir(const valarray& b, const valarray& a, const valarray& x, valarray& res);

void filter_hpf(valarray& x, const double fc);

void filter_lpf(valarray& x, const double fc);


#endif // FILTER_H
