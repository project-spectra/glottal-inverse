#include "operators.h"
#include "operators_buffer.h"

#include <zstd.h>


OperatorBuffer::OperatorBuffer(size_t cap)
    : m_capacity(cap), m_mats(cap)
{
    for (auto& it : m_mats) {
        //it = std::make_pair(false, gsl_matrix_alloc(basisLength, basisLength));
    }

    readCompressedFiles();
}

OperatorBuffer::~OperatorBuffer() {
    for (auto& it : m_mats) {
        //gsl_matrix_free(it.second);
    }

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

    gsl_matrix *dense = gsl_matrix_alloc(basisLength, basisLength);

    gsl_spmatrix_sp2d(dense, sparse);

    gsl_spmatrix_free(sparse);

    return dense;
    
    /*size_t it_mu, k;

    gsl_matrix *denseMat;
    
    // look for it, if it's loaded, good.
    for (auto& entry : m_buffer)
    {
        std::tie(it_mu, k) = entry;

        if (it_mu == mu) {
            return m_mats[k].second;
        }
    }

    // if not, check capacity and drop the end.
    if (m_buffer.size() == m_capacity) {
        // reset mat to zero
        size_t k = m_buffer.back().second;
        
        auto& matEntry = m_mats[k];
        matEntry.first = false; // unused
        gsl_matrix_set_zero(matEntry.second);

        m_buffer.pop_back();
    }
    
    // find first free matrix
    for (k = 0; k < m_capacity; ++k) {
        auto& matEntry = m_mats[k];

        // unused
        if (!matEntry.first) {    
            matEntry.first = true;
            denseMat = matEntry.second;
            break;
        }
    }

    // get matrix and load it
    gsl_spmatrix *sparseMat = loadMat(mu);
    gsl_spmatrix_sp2d(denseMat, sparseMat);
    gsl_spmatrix_free(sparseMat);

    m_buffer.emplace_front(mu, k);

    return denseMat;*/

}


