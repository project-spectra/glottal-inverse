#ifndef OPERATORS_BUFFER_H
#define OPERATORS_BUFFER_H


#include <deque>
#include <mutex>

#include "operators.h"


void spmatrix_read(gsl_spmatrix *dest, const void *src);


class OperatorBuffer {
public:
    OperatorBuffer(size_t cap = 16);
    virtual ~OperatorBuffer();

    gsl_matrix *get(size_t mu);

private:
    // Load compressed files
    void readCompressedFiles();

    ////

    // maximum size of ring buffer
    size_t m_capacity;
    
    // pair is  (mu, mat_k)
    std::deque<std::pair<size_t, size_t>> m_buffer;

    // pair is  (used, mat)
    vector<std::pair<bool, gsl_matrix *>> m_mats;

    // load *all* the compressed data in-memory
    vector<std::pair<size_t, void *>> m_data;
};


#endif // OPERATORS_BUFFER_H
