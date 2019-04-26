#include <numeric>
#include <complex>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigenvalues>

#include "poly.h"
#include "filter.h"

using namespace Eigen;
using complex = std::complex<double>;

std::array<valarray, 2> hpf_butter(size_t n, double fc, double fs) {
    double Wn = fc / (fs / 2.);

    VectorXcd z = {};
    VectorXcd p = ArrayXcd(1.j * (M_PI * ArrayXcd::LinSpaced(n-1, 1, 2*n-1) / (2. * n) + M_PI / 2.)).exp();
    complex k = (-p).prod().real();
   
    // Convert the zero pole gain to transfer function representation
    VectorXcd num(k * poly(z));
    VectorXcd den(poly(p));

    VectorXcd numPadded(den.size()); // left-pad b with zeros
    numPadded.tail(num.size()) = num;

    // Convert the transfer function representation to state-space representation
    size_t ns = den.size() - 1;

    MatrixXcd A(ns, ns);
    VectorXcd B(ns);
    RowVectorXcd C(ns);
    complex D;

    numPadded /= den[0];
    den /= den[0];

    RowVectorXcd numShf = numPadded.segment(1, ns).transpose();
    RowVectorXcd denShf = den.segment(1, ns).transpose();

    A.row(0) = -denShf;
    A.bottomLeftCorner(ns - 1, ns - 1) = MatrixXcd::Identity(ns - 1, ns - 1);
    A.bottomRightCorner(ns - 1, 1) = MatrixXcd::Zero(ns - 1, 1);

    B[0] = 1.;
    B.tail(ns - 1) = VectorXcd::Zero(ns - 1);
    C = numShf - num[0] * denShf;
    D = num[0];

    // Transform the low-pass filter to a high-pass filter
    //  At = Wn * inv(A)
    //  Bt = -Wn * (inv(A) * B) = -At * B
    //  Ct = inv(C) * A
    //  Dt = D - inv(C) * A * B = D - Ct * B
    MatrixXcd invA = A.inverse();

    MatrixXcd At = Wn * invA;
    VectorXcd Bt = -At * B;
    RowVectorXcd Ct = C * invA;
    complex Dt = D - (Ct * B).sum();

    // Transform the analog high-pass filter to a digital discrete high-pass filter
    double lambda(fs);
    
    complex one2l(1. / (2. * lambda));
    complex rsqrl(-1. / sqrt(lambda));

    auto Id = MatrixXcd::Identity(ns, ns);
    auto invImA = (Id - one2l * At).inverse();

    auto CtinvImA = Ct * invImA;

    auto Ad = invImA * (Id + one2l * At);
    auto Bd = rsqrl * invImA * Bt;
    auto Cd = rsqrl * CtinvImA;
    auto Dd = one2l * (CtinvImA * Bt).sum() + Dt;

    // Convert the digital state-space filter representation back to transfer function
    ComplexEigenSolver<MatrixXcd> solver;

    solver.compute(Ad, false);
    VectorXcd denPolyC(poly(solver.eigenvalues()));

    solver.compute(Ad - Bd * Cd, false);
    VectorXcd numPolyC((poly(solver.eigenvalues()) + (Dd - 1.) * denPolyC)); 

    VectorXd denPoly = denPolyC.real();
    VectorXd numPoly = numPolyC.real();

    valarray resB(numPoly.data(), numPoly.size());
    valarray resA(denPoly.data(), denPoly.size());

    return { resB, resA };

}
