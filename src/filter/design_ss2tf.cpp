#include "poly.h"
#include "filter_design.h"

using namespace arma;

void convert_ss2tf(const filter_ss& src, filter_tf& dst)
{
    cx_vec b_eig = eig_gen(src.A - src.B * src.C);
    cx_vec a_eig = eig_gen(src.A);

    dst.a = poly(a_eig);
    dst.b = poly(b_eig) + (src.D - 1.) * dst.a;
}
