#include "hwt.h"


const auto Hwt = hwt_transform();
const auto Hwt_inv = hwt_inverse();


static gsl_matrix *hwt_transform() {

    static double matArray[J * J];
    static auto matView = gsl_matrix_view_array(matArray, J, J);
    static auto mat = &matView.matrix;

    const double x = sqrt(2.) / 2.;
    double y1, y2;
    size_t i, j;

    for (i = 0; i < J / 2; ++i) {
        for (j = 0; j < J; ++j) {
            if (j == i) {
                y1 = x;
                y2 = -x;
            } else if (j == i + 1) {
                y1 = y2 = x;
            } else {
                y1 = y2 = 0.;
            }
        
            gsl_matrix_set(mat, i, j, y1);
            gsl_matrix_set(mat, i + J / 2, j, y2);
        }
    }

    return mat;

}

static gsl_matrix *hwt_inverse() {

    static auto mat = gsl_matrix_alloc(J, J);

    gsl_matrix_transpose_memcpy(mat, hwt_transform());

    return mat;

}



