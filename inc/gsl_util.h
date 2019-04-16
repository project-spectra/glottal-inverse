#ifndef GSL_UTIL_H
#define GSL_UTIL_H


#include <vector>
#include <array>

#include "constants.h"

// basisLength
#define static_vector(name)  \
    static std::array<double, basisLength> arr_##name;  \
    static auto view_##name = gsl_vector_view_array(arr_##name.data(), basisLength);  \
    static auto name = &view_##name.vector;  \

#define static_matrix(name)  \
    static std::array<double, basisLength * basisLength> arr_##name;  \
    static auto view_##name = gsl_matrix_view_array(arr_##name.data(), basisLength, basisLength);  \
    static auto name = &view_##name.matrix;  \


#define static_vector2(name, size)  \
    static std::vector<double> arr_##name(size);  \
    static auto view_##name = gsl_vector_view_array(arr_##name.data(), size);  \
    static auto name = &view_##name.vector;  \

#define static_matrix2(name, size1, size2)  \
    static std::vector<double> arr_##name(size1 * size2);  \
    static auto view_##name = gsl_matrix_view_array(arr_##name.data(), size1, size2);  \
    static auto name = &view_##name.matrix;  \


#endif // GSL_UTIL_H
