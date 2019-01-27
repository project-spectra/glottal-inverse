#include "linalg.h"

#include <iostream>
#include <algorithm>


// Discrete integration: Riemann sum
double integrate(gsl_vector *f) {
    static constexpr size_t length = WINDOW_LENGTH * SAMPLE_RATE;
    static double unit[length];
    static auto unitView = gsl_vector_view_array(unit, length);

    double res;
    
    gsl_vector_set_all(&unitView.vector, 1.);
    gsl_blas_ddot(&unitView.vector, f, &res);

    return SAMPLE_PER * res;
}
