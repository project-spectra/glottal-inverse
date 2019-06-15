#include "poly.h"
#include "filter_design.h"


void convert_zpk2tf(const filter_zpk& src, filter_tf& dst)
{
    dst.b = src.k * poly(src.z);
    dst.a = poly(src.p);
}
