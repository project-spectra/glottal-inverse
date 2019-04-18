#include "operators.h"
#include "operators_buffer.h"

#include <zstd.h>


gsl_matrix *OperatorBuffer::readOneMatrix(size_t mu)
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

    gsl_matrix *dense = gsl_matrix_alloc(basisLength, basisLength);
    gsl_spmatrix_sp2d(dense, sparse);

    gsl_spmatrix_free(sparse);

    return dense;
}

void OperatorBuffer::decompressFiles()
{
    m_mats.resize(basisLength);

    for (size_t mu = 0; mu < basisLength; ++mu) {
        m_mats[mu] = readOneMatrix(mu);
    }
}
