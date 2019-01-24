#include "lpc.h"
#include "filter.h"


gsl_vector *lpcResidual(
    gsl_vector *x,
    size_t numSamples,
    size_t L,
    size_t shift,
    size_t order
) {
    
    size_t start, n;
    gsl_vector *win, *A, *inv, *res;

    res = gsl_vector_calloc(numSamples);
    
    start = 1;
    n = 1;
    win = hanning(L+1);
    
    for (start = n = 1; start < numSamples - shift; ++n, start += shift) {
        auto segX = gsl_vector_subvector(x, start, L);
        auto segRes = gsl_vector_subvector(res, start, L);
        
        gsl_vector_mul(&segX.vector, win);
        
        A = lpcCoeffs(&segX.vector, L, order);

        inv = filter_fir(A, order + 1, &segX.vector, L);
        gsl_vector_scale(inv, gsl_blas_dnrm2(inv) / gsl_blas_dnrm2(&segX.vector));

        gsl_vector_add(&segRes.vector, inv);
       
        gsl_vector_free(A);
        gsl_vector_free(inv);
    }
  
    // Normalize amplitude
    double min, max;
    gsl_vector_minmax(res, &min, &max);
    double amp = GSL_MAX_DBL(abs(min), abs(max));
    gsl_vector_scale(res, 1 / amp);

    gsl_vector_free(win);

    return res;
}
