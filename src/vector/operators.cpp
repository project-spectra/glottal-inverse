#include "vector.h"
#include "linalg.h"


Vector::operator bool() const {
    return m_vec != nullptr;
}

void Vector::operator *=(const Vector& other) {
    gsl_vector_mul(m_vec, other.m_vec);
}

double Vector::operator [](const size_t i) const {
    return gsl_vector_get(m_vec, i);
}

double& Vector::operator [](const size_t i) {
    return *gsl_vector_ptr(m_vec, i);
}


void Vector::coords() {
    projForward(m_vec);
}

void Vector::uncoords() {
    projBackward(m_vec);
}

void Vector::basis(size_t i) {
    gsl_vector_set_basis(m_vec, i);
}
