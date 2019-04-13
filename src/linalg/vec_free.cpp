#include "linalg.h"


const VectorFree vecFree;


void VectorFree::operator()(gsl_vector *x) const noexcept {
    
    if (x != nullptr) {
        gsl_vector_free(x);
    }

}
