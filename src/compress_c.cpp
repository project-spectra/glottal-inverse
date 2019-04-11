#include "restore_c.h"


gsl_spmatrix *cpWriteMat(FILE *file, gsl_spmatrix *mat) {    

    size_t size1(mat->size1);
    size_t size2(mat->size2);
    size_t nz(mat->nz);

    // Calculate data buffer size.
    size_t workSize(GSL_MAX(size1, size2) *
                    GSL_MAX(sizeof(size_t), sizeof(double)));

    size_t dataSize(
        4 * sizeof(size_t) +
          nz * sizeof(size_t) +
          nz * sizeof(double) +
          (size2 + 1) * sizeof(size_t) +
          workSize
    );
     
    auto ccs = gsl_spmatrix_ccs(mat);
    
    auto data = make_unique<unsigned char[]>(dataSize);
    void *cur = data.get();
    
    cur = mempcpy(cur, &ccs->size1, sizeof(size_t));
    cur = mempcpy(cur, &ccs->size2, sizeof(size_t));
    cur = mempcpy(cur, &ccs->nzmax, sizeof(size_t));
    cur = mempcpy(cur, &ccs->nz, sizeof(size_t));

    cur = mempcpy(cur, ccs->i, nz * sizeof(size_t));
    cur = mempcpy(cur, ccs->data, nz * sizeof(double));
    cur = mempcpy(cur, ccs->p, (size2 + 1) * sizeof(size_t));
    cur = mempcpy(cur, ccs->work, workSize);

    // Try to compress
    size_t compressedSize(dataSize);
    auto buffer = make_unique<unsigned char[]>(compressedSize);

    compressBound(compressedSize);
    int ret = compress2(buffer.get(), &compressedSize, data.get(), dataSize, 6); 
    if (ret != Z_OK) {
        fclose(file);
        throw std::ios_base::failure("Couldn't compress matrix");
    }

    // Write header for total lengths
    fwrite(&dataSize, sizeof(size_t), 1, file);       // data
    fwrite(&compressedSize, sizeof(size_t), 1, file); // compressed

    fwrite(buffer.get(), sizeof(unsigned char), compressedSize, file);

    return ccs;
}
