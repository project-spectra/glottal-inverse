#include "restore_c.h"

gsl_spmatrix *cpReadMat(const char *pathRaw, const char *pathCompressed) {

    // First decompress the file.

    FILE *fInfIn = fopen(pathCompressed, "rb");
    FILE *fInfOut = fopen(pathRaw, "wb");

    int ret = inf(fInfIn, fInfOut);

    fclose(fInfIn);
    fclose(fInfOut);

    if (ret != Z_OK) {
        zerr(ret);
        exit(EXIT_FAILURE);
    }
   
    // Then read the matrix.
   
    FILE *fileGsl = fopen(pathRaw, "rb");

    size_t nnz;
    fread(&nnz, sizeof nnz, 1, fileGsl);

    auto mat = gsl_spmatrix_alloc_nzmax(length, length, nnz, GSL_SPMATRIX_CCS);

    gsl_spmatrix_fread(fileGsl, mat);

    fclose(fileGsl);

    return mat;
}



