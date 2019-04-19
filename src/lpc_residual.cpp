#include "lpc.h"
#include "filter.h"

#include <gsl/gsl_blas.h>


void lpcResidual(gsl_matrix *lpc, gsl_vector *res, gsl_vector *x, size_t L, size_t shift, size_t order)
{
    const size_t lenX(x->size);
    const size_t N(L + 1);
    size_t start(0);
    size_t stop(start + L); 
    size_t n;

    auto win = hanning(N);
    
    //  Assumed:
    // dim(lpc) = (order+1, len(x))
    // len(res) = len(x)
    // len(lpc) = len(x)/shift

    n = 1;

    gsl_vector *segment = gsl_vector_alloc(N);
    gsl_vector *inv = gsl_vector_alloc(N);

    double lpcRow[order+1];

    while (stop < lenX)
    {
        auto segView = gsl_vector_subvector(x, start, N);
        gsl_vector_memcpy(segment, &segView.vector);
        gsl_vector_mul(segment, win);

        lpcCoeffs(lpcRow, segment, order);

        auto lpcRowView = gsl_vector_view_array(lpcRow, order+1);
        filter_fir(&lpcRowView.vector, segment, inv);

        if (lpc != nullptr) {
            auto matRow = gsl_matrix_row(lpc, n);
            gsl_vector_memcpy(&matRow.vector, &lpcRowView.vector);
        }

        double Knum(0.), Kden(0.);
        double data;

        for (size_t k = 0; k < N; ++k) {
            data = gsl_vector_get(segment, k);
            Knum += data * data;

            data = gsl_vector_get(inv, k);
            Kden += data * data;
        }

        gsl_vector_scale(inv, sqrt(Knum / Kden));
        gsl_vector_add(res, inv);

        start += shift;
        stop += shift;
        n++;
    }

    double max = gsl_vector_get(res, gsl_blas_idamax(res));
    gsl_vector_scale(res, 1. / abs(max));
}
