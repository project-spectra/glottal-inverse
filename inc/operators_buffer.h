#ifndef OPERATORS_BUFFER_H
#define OPERATORS_BUFFER_H


#include <deque>
#include <mutex>

#include "operators.h"


void spmatrix_read(gsl_spmatrix *dest, const void *src);


class OperatorBuffer {
public:
    OperatorBuffer();
    virtual ~OperatorBuffer();

    gsl_matrix *get(size_t mu);

private:
    // Load compressed files
    void readCompressedFiles();

    // load all the compressed files in-memory
    vector<std::pair<size_t, void *>> m_data;

    // single matrix (computation is on a single thead)
    gsl_matrix *m_mat;
};


#endif // OPERATORS_BUFFER_H
