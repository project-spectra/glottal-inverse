#ifndef VEC_WRAPPER_H
#define VEC_WRAPPER_H


#include <atomic>

#include <gsl/gsl_vector.h>


class Vector {
public:
    Vector(); // null

    Vector(gsl_vector *wrap);
    Vector(const size_t length);
    Vector(const Vector& copy);
    virtual ~Vector();

    gsl_vector *get() const;
    void set(gsl_vector *newVector);
    void reset(gsl_vector *newVector);

    void reset(const size_t length);


    explicit operator bool() const;

    Vector &operator =(const Vector& copy);
    void operator *=(const Vector& other);
    double operator [](const size_t i) const;
    double& operator [](const size_t i);


    void coords();
    void uncoords();
    void basis(size_t i);

private:
    gsl_vector *m_vec;

    std::atomic<unsigned int> m_delCount;
};


#endif // VEC_WRAPPER_H
