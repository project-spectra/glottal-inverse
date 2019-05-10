#include <numeric>
#include <complex>
#include <armadillo>

#include "poly.h"
#include "filter.h"

using namespace arma;


std::array<valarray, 2> hpf_butter(size_t n, double fc, double fs) {
    double Wn = fc / (fs / 2.);

    cx_vec z = {};
    cx_vec p = exp(cx_double(0., 1.)  * (M_PI * regspace<cx_vec>(1, 2, 2*n-1) / (2. * n) + M_PI / 2.));
    cx_double k = real(prod(-p));
   
    // Convert the zero pole gain to transfer function representation
    cx_vec num = { k };
    cx_vec den = poly(p);

    cx_vec numPadded(den.n_elem); // left-pad b with zeros
    numPadded(span(den.n_elem - num.n_elem, den.n_elem - 1)) = num;

    // Convert the transfer function representation to state-space representation
    uword ns = den.n_elem - 1;

    cx_mat A(ns, ns);
    cx_vec B(ns);
    cx_rowvec C(ns);
    cx_double D;

    numPadded /= den[0];
    den /= den[0];

    cx_rowvec numShf = numPadded(span(1, ns)).t();
    cx_rowvec denShf = den(span(1, ns)).t();

    A.row(0) = -denShf;
    A(span(1, ns - 1), span(0, ns - 2)) = eye<cx_mat>(ns - 1, ns - 1);
    A(span(1, ns - 1), ns - 1) = zeros<cx_vec>(ns - 1);

    B[0] = 1.;
    B.tail(ns - 1) = zeros<cx_vec>(ns - 1);
    C = numShf - num[0] * denShf;
    D = num[0];

    // Transform the low-pass filter to a high-pass filter
    //  At = Wn * inv(A)
    //  Bt = -Wn * (inv(A) * B) = -At * B
    //  Ct = inv(C) * A
    //  Dt = D - inv(C) * A * B = D - Ct * B
    cx_mat invA = inv(A);

    cx_mat At = Wn * invA;
    cx_vec Bt = -At * solve(A, B);
    cx_rowvec Ct = C * invA;
    cx_double Dt = D - sum(Ct * B);

    // Transform the analog high-pass filter to a digital discrete high-pass filter
    double lambda(fs);
    
    cx_double one2l(1. / (2. * lambda));
    cx_double rsqrl(-1. / sqrt(lambda));

    cx_mat invImA = (eye<cx_mat>(ns, ns) - one2l * At).i();
    cx_mat IpA = eye<cx_mat>(ns, ns) + one2l * At;

    cx_rowvec CtinvImA = Ct * invImA;

    cx_mat Ad = invImA * IpA;
    cx_vec Bd = rsqrl * invImA * Bt;
    cx_rowvec Cd = rsqrl * CtinvImA;
    cx_double Dd = one2l * sum(CtinvImA * Bt) + Dt;

    // Convert the digital state-space filter representation back to transfer function

    cx_vec denEigen = eig_gen(Ad);
    cx_vec numEigen = eig_gen(Ad - Bd * Cd);

    cx_vec denPolyC = poly(denEigen);
    cx_vec numPolyC = poly(numEigen) + (Dd - 1.) * denPolyC;

    vec denPoly = real(denPolyC);
    vec numPoly = real(numPolyC);

    valarray resB(numPoly.memptr(), numPoly.n_elem);
    valarray resA(denPoly.memptr(), denPoly.n_elem);

    return { resB, resA };

}
