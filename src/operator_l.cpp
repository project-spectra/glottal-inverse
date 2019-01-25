#include "amgif.h"
#include "filter.h"
#include "interp_sample.h"
#include "linalg.h"


constexpr double besselCoeffs[][6] = {
    { 1, 1 },
    { 3, 3, 1 },
    { 15, 15, 6, 1 },
    { 105, 105, 45, 10, 1},
    { 945, 945, 420, 105, 15, 1 }
};

static inline gsl_vector *applyBessel(gsl_vector *x, size_t order) {
    auto b = gsl_vector_const_view_array(besselCoeffs[order-1], 1);
    auto a = gsl_vector_const_view_array(besselCoeffs[order-1], order+1);

    return filter_iir(&b.vector, &a.vector, x);
}


mat_operator computeL() {
    gsl_matrix *L;
    gsl_vector *H;
    gsl_function scaling;
    size_t index, i, j;

    constexpr size_t length = basis_length();
    
    L = gsl_matrix_alloc(length, length);

    index = 0;
    scaling.function = basis_eval;
    scaling.params = &index;

    H = coords(&scaling);

    double data;
    
    for (i = 0; i < length; ++i) {
        for (j = 0; j < length; ++j) {
            data = gsl_vector_get(H, abs((int) i - (int) j));
            gsl_matrix_set(L, i, j, data);
        } 
    }

    gsl_vector_free(H);

    return mat_operator(L, gsl_matrix_free);
}
