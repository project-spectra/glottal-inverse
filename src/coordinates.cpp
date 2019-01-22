#include "linalg.h"


double coords_eval(double t, void *p) {
    auto U = static_cast<gsl_vector *>(p);
    gsl_function phi;
    size_t index;

    double sum, K;
    
    phi.function = basis_eval;
    phi.params = &index;
    
    sum = 0;

    for (index = 0; index < basis_length(); ++index) {
        K = gsl_vector_get(U, index);
        sum += K * GSL_FN_EVAL(&phi, t);
    }

    return sum;
}

gsl_vector *coords(gsl_function *f) {
    size_t length, index;
    gsl_function phi;

    gsl_vector *U;
    double data;

    length = basis_length();

    phi.function = basis_eval;
    phi.params = &index;

    U = gsl_vector_alloc(length);

    for (index = 0; index < length; ++index) {
        data = projection(f, &phi);
        gsl_vector_set(U, index, data);
    }

    return U;
}
