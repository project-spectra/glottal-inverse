#include "vector.h"


gsl_vector *Vector::get() const {
    return m_vec;
}

void Vector::set(gsl_vector *newVector) {
    m_vec = newVector;
}

void Vector::reset(gsl_vector *newVector) {
    m_vec = newVector;
}

void Vector::reset(size_t length) {
    m_vec = gsl_vector_alloc(length);
}
