#include "operators.h"
#include "operators_restore.h"

gsl_spmatrix *loadMat(size_t mu) {

    // Get filenames.

    string raw, compressed;

    setFilenames(raw, compressed, mu);

    // First decompress the file.

    fs::create_directories(PERSIST_DIR);

    FILE *fInfIn = fopen(compressed.c_str(), "rb");
    FILE *fInfOut = fopen(raw.c_str(), "wb");

    int ret = inf(fInfIn, fInfOut);

    fclose(fInfIn);
    fclose(fInfOut);

    if (ret != Z_OK) {
        zerr(ret);
        exit(EXIT_FAILURE);
    }
   
    // Then read the matrix.
   
    FILE *fileGsl = fopen(raw.c_str(), "rb");

    size_t nnz;
    fread(&nnz, sizeof nnz, 1, fileGsl);

    auto C = gsl_spmatrix_alloc_nzmax(basisLength, basisLength, nnz, GSL_SPMATRIX_CCS);

    gsl_spmatrix_fread(fileGsl, C);

    fclose(fileGsl);

    fs::remove(raw);

    return C;
}



