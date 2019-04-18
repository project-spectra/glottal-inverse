#ifndef OPERATORS_BUFFER_H
#define OPERATORS_BUFFER_H


#include "operators.h"


void spmatrix_read(gsl_spmatrix *dest, const void *src);


class OperatorBuffer {
public:
    OperatorBuffer();
    virtual ~OperatorBuffer();

    gsl_matrix **get();

private:
    // Load compressed files
    void readCompressedFiles();

    // Decompress files
    void decompressFiles();

    // Decompress one matrix
    gsl_matrix *readOneMatrix(size_t mu);

    // load all the compressed files in-memory
    vector<std::pair<size_t, void *>> m_data;

    // matrix buffer (computiation is on a single thread)
    vector<gsl_matrix *> m_mats;
    
};


#endif // OPERATORS_BUFFER_H
