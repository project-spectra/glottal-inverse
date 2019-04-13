#include "operators.h"
#include "operators_restore.h"

void storeMat(const gsl_spmatrix *C, size_t mu) {

    // Get names.
    
    string raw, compressed;

    setFilenames(raw, compressed, mu);

    // First write the matrix in raw file.

    fs::create_directories(PERSIST_DIR);

    FILE *fileGsl = fopen(raw.c_str(), "wb");

    size_t nnz = gsl_spmatrix_nnz(C);
    fwrite(&nnz, sizeof nnz, 1, fileGsl);

    gsl_spmatrix_fwrite(fileGsl, C);

    fclose(fileGsl);

    // Then write to compressed file.

    FILE *fDefIn = fopen(raw.c_str(), "rb");
    FILE *fDefOut = fopen(compressed.c_str(), "wb");

    //int ret = def(fDefIn, fDefOut, Z_DEFAULT_COMPRESSION);
    int ret = def(fDefIn, fDefOut, 5);
    
    fclose(fDefIn);
    fclose(fDefOut);

    fs::remove(raw);

    if (ret != Z_OK) {
        zerr(ret);
        exit(EXIT_FAILURE);
    }
}
