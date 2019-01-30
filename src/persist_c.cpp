#include "operator_c.h"
#include "persist_c.h"


gsl_spmatrix *smartGetC(size_t mu) {
    constexpr size_t length = 2 << J;
    char path[PERSIST_PATHLEN];
    FILE *f;
    gsl_spmatrix *C;

    snprintf(path, PERSIST_PATHLEN, "%s/%zu", PERSIST_DIR, mu);

    if (fs::is_directory(PERSIST_DIR) && fs::exists(path)) {
        // if the directory already exists, simply restore.
        f = fopen(path, "rb");
        C = gsl_spmatrix_fscanf(f);
        fclose(f);
    } else {
        // else, compute and then persist.
        fs::create_directories(PERSIST_DIR);

        C = gsl_spmatrix_alloc(length, length);

        computeSingleC(C, mu);

        f = fopen(path, "wb");
        gsl_spmatrix_fprintf(f, C, "%g");
        fclose(f);
    }
    
    return C;
}

