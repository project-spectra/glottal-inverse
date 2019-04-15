#include "operators.h"
#include "operators_buffer.h"

#include <iostream>
#include <zstd.h>


OperatorBuffer::OperatorBuffer()
    : m_mat(gsl_matrix_alloc(basisLength, basisLength))
{
    readCompressedFiles();
}

OperatorBuffer::~OperatorBuffer() {
    gsl_matrix_free(m_mat);

    for (auto& it : m_data) {
        free(it.second);
    }
}

gsl_matrix *OperatorBuffer::get(size_t mu)
{
    // get compressedData
    void *bufSrc;
    size_t bufLen;
    
    std::tie(bufLen, bufSrc) = m_data[mu];

    const size_t destLen = ZSTD_getFrameContentSize(bufSrc, bufLen);
    if (ZSTD_isError(destLen)) {
        fprintf(stderr, "Error getFrameContentSize C[%zu] : %s\n", mu, ZSTD_getErrorName(destLen));
        exit(EXIT_FAILURE);
    }

    void *bufDest = new unsigned char[destLen];

    const size_t bytesDecompressed = ZSTD_decompress(bufDest, destLen, bufSrc, bufLen);
    if (ZSTD_isError(bytesDecompressed)) {
        fprintf(stderr, "Error ZSTD_decompress C[%zu] : %s\n", mu, ZSTD_getErrorName(bytesDecompressed));
        exit(EXIT_FAILURE);
    }

    gsl_spmatrix *sparse = gsl_spmatrix_alloc(basisLength, basisLength);
    spmatrix_read(sparse, bufDest);

    gsl_matrix_set_zero(m_mat);
    gsl_spmatrix_sp2d(m_mat, sparse);

    gsl_spmatrix_free(sparse);

    return m_mat;
}
