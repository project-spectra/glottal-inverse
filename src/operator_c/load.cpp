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

    decompress(fInfIn, fInfOut);

    fclose(fInfIn);
    fclose(fInfOut);
   
    // Then read the matrix.
   
    FILE *fileGsl = fopen(raw.c_str(), "rb");

    size_t nnz;
    fread(&nnz, sizeof nnz, 1, fileGsl);

    auto C = gsl_spmatrix_alloc(basisLength, basisLength);

    gsl_spmatrix_fread(fileGsl, C);

    fclose(fileGsl);

    fs::remove(raw);

    return C;
}



