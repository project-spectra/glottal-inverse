#include "filter_design.h"

using namespace arma;

void convert_tf2ss(const filter_tf& src, filter_ss& dst)
{
    // Left-pad numerator with zeros
    cx_vec b(zeros<cx_vec>(src.a.n_elem));
    b.tail(src.b.n_elem) = src.b;
    
    cx_vec a(src.a);
    
    // Normalize the rational fraction
    b /= a(0);
    a /= a(0);

    // Conversion.
    const uword ns = src.a.n_elem - 1;

    dst.A.reshape(ns, ns);
    dst.A.row(0) = -a(span(1, ns)).t();
    dst.A(span(1, ns - 1), span(0, ns - 2)) = eye<cx_mat>(ns - 1, ns - 1);
    dst.A(span(1, ns - 1), ns - 1) = zeros<cx_vec>(ns - 1);
    
    dst.B.resize(ns);
    dst.B(0) = 1.;
    dst.B.tail(ns - 1) = zeros<cx_vec>(ns - 1);

    dst.C.resize(ns);
    dst.C = b(span(1, ns)).t() - b(0) * a(span(1, ns)).t();
    
    dst.D = b(0);
}
