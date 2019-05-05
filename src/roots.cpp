#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigenvalues>

#include <iostream>

#include "roots.h"


using namespace Eigen;

cvalarray roots(const valarray& p)
{
    int n = p.size() - 1;

    MatrixXd A(n, n);
    A.setZero();
    A.diagonal(-1).setOnes();
    
    for (int i = 0; i < n; ++i) {
        A(0, i) = -p[i+1] / p[0];
    }

    ComplexEigenSolver<MatrixXd> solver;

    solver.compute(A, false);

    VectorXcd rts(solver.eigenvalues());

    return cvalarray(rts.data(), rts.size());

}
