#include "filter_design.h"

using namespace arma;

void transform_lp2lp(const filter_ss& src, double Wo, filter_ss& dst)
{
    dst.A = Wo * src.A;
    dst.B = Wo * src.B;
    dst.C = src.C;
    dst.D = src.D;
}

void transform_lp2hp(const filter_ss& src, double Wo, filter_ss& dst)
{
    cx_mat invA = inv(src.A);

    dst.A = Wo * invA;
    dst.B = -Wo * solve(src.A, src.B);
    dst.C = src.C * invA;
    dst.D = src.D - sum(dst.C * src.B);
}
