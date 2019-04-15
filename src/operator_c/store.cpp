#include "operators.h"
#include "operators_restore.h"

void storeMat(const gsl_spmatrix *C, size_t mu) {

    // Get names.
    
    string raw, compressed;

    setFilenames(raw, compressed, mu);

    // First write the matrix in raw file.

    fs::create_directories(PERSIST_DIR);

    FILE *fileGsl = fopen(raw.c_str(), "wb");

    gsl_spmatrix_fwrite(fileGsl, C);

    fclose(fileGsl);

    // Then write to compressed file.

    FILE *fDefIn = fopen(raw.c_str(), "rb");
    FILE *fDefOut = fopen(compressed.c_str(), "wb");

    compress(fDefIn, fDefOut, 3);
    
    fclose(fDefIn);
    fclose(fDefOut);

    fs::remove(raw);
}
