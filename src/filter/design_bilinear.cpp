#include "audio.h"
#include "filter_design.h"

using namespace arma;

static void bilinear_lambda(const filter_ss& src, double lambda, filter_ss& dst)
{
    const uword ns(src.B.n_elem);

    cx_double one2l(1. / (2. * lambda));
    cx_double rsqrl(-1. / sqrt(lambda));

    cx_mat invImA = (eye<cx_mat>(ns, ns) - one2l * src.A).i();
    cx_mat IpA = eye<cx_mat>(ns, ns) + one2l * src.A;

    cx_rowvec CtinvImA = src.C * invImA;

    dst.A = invImA * IpA;
    dst.B = rsqrl * invImA * src.B;
    dst.C = rsqrl * CtinvImA;
    dst.D = one2l * sum(CtinvImA * src.B) + src.D;
}

void convert_bilinear(const filter_ss& src, filter_ss& dst)
{
    const double fs(SAMPLE_RATE);

    bilinear_lambda(src, fs, dst);
}

void convert_bilinear(const filter_ss& src, double fp, filter_ss& dst)
{
    const double fs(SAMPLE_RATE);
    const double lambda = (M_PI * fp) / tan((M_PI * fp) / fs);

    bilinear_lambda(src, lambda, dst);
}
