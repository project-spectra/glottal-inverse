#include "restore_c.h"

void cpWriteMat(const char *pathRaw, const char *pathCompressed, gsl_spmatrix *mat) {
  
    // First write the matrix in raw file.

    FILE *fileGsl = fopen(pathRaw, "wb");

    size_t nnz = gsl_spmatrix_nnz(mat);
    fwrite(&nnz, sizeof nnz, 1, fileGsl);

    gsl_spmatrix_fwrite(fileGsl, mat);

    fclose(fileGsl);

    // Then write to compressed file.

    FILE *fDefIn = fopen(pathRaw, "rb");
    FILE *fDefOut = fopen(pathCompressed, "wb");

    //int ret = def(fDefIn, fDefOut, Z_DEFAULT_COMPRESSION);
    int ret = def(fDefIn, fDefOut, Z_BEST_SPEED);
    
    fclose(fDefIn);
    fclose(fDefOut);

    if (ret != Z_OK) {
        zerr(ret);
        exit(EXIT_FAILURE);
    }
}
