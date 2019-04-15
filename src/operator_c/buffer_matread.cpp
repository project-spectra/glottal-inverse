#include "operators_buffer.h"

#include <cstring>


void spmatrix_read(gsl_spmatrix *dest, const void *src)
{

    auto cur = static_cast<const unsigned char *>(src);

    size_t size1, size2, nz;

    memcpy(&size1, cur, sizeof(size_t));
    cur += sizeof(size_t);

    memcpy(&size2, cur, sizeof(size_t));
    cur += sizeof(size_t);

    memcpy(&nz, cur, sizeof(size_t));
    cur += sizeof(size_t);

    // set nzmax
    gsl_spmatrix_realloc(nz, dest);

    // read i, data, p, wihch are all size nz, for a triplet storage format.
    memcpy(dest->i, cur, nz * sizeof(size_t));
    cur += nz * sizeof(size_t);

    memcpy(dest->data, cur, nz * sizeof(double));
    cur += nz * sizeof(double);

    memcpy(dest->p, cur, nz * sizeof(size_t));
    
    // success.
}
