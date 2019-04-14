#include "vector.h"
#include <cstdio>

Vector::Vector()
    : m_vec(nullptr), m_delCount(0)
{
}

Vector::Vector(gsl_vector *wrap)
    : m_vec(gsl_vector_alloc(wrap->size)),
      m_delCount(0)
{
    gsl_vector_memcpy(m_vec, wrap);
}

Vector::Vector(const size_t length)
    : m_vec(gsl_vector_alloc(length)),
      m_delCount(0)
{
}

Vector::Vector(const Vector& copy)
    : Vector(copy.m_vec)
{
}

Vector::~Vector() {
    if (++m_delCount < 1 && m_vec != nullptr) {
        gsl_vector_free(m_vec);
        m_vec = nullptr;
    }
}

Vector &Vector::operator =(const Vector& copy) {
    reset(copy.m_vec);
    return *this;
}
