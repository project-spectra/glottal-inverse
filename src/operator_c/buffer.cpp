#include "operators.h"
#include "operators_buffer.h"


OperatorBuffer::OperatorBuffer(size_t cap)
    : m_capacity(cap), m_mats(cap)
{
    for (auto& it : m_mats) {
        it = std::make_pair(false, gsl_matrix_alloc(basisLength, basisLength));
    }
}

OperatorBuffer::~OperatorBuffer() {
    for (auto& it : m_mats) {
        gsl_matrix_free(it.second);
    }
}

gsl_matrix *OperatorBuffer::get(size_t mu)
{
    size_t it_mu, k;

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
    spmatrix_ccs2d(denseMat, sparseMat);
    gsl_spmatrix_free(sparseMat);

    m_buffer.emplace_front(mu, k);

    return denseMat;

}


void spmatrix_ccs2d(gsl_matrix *dest, gsl_spmatrix *src)
{
    const size_t *si(src->i);
    const size_t *sp(src->p);
    const double *sdata(src->data);

    const size_t size2(src->size2);

    size_t i, j, p;    
    size_t sp_jp1;

    double data;

    // if zero, zero
    if (src->nz == 0) return;

    // for each column
    for (j = 0; j < size2; ++j) {
        sp_jp1 = sp[j + 1];

        // find each row with data
        for (p = sp[j]; p < sp_jp1; ++p) {
            i = si[p];
            data = sdata[p];

            gsl_matrix_set(dest, i, j, data);
        }
    }
}
