#include "persist_c.h"


vector<mat_operator> smartGetC() {
    vector<mat_operator> C;

    // if the directory already exists, simply restore.
    if (fs::is_directory(PERSIST_DIR)) {
        std::cout << "  * Loading precomputed matrices" << std::endl;
        if (restoreC(C)) {
            return C;
        }
    }
    
    // else, compute and then persist.
    C = computeC();

    std::cout << "  * Saving computed matrices for further runs" << std::endl;
    persistC(C);
    
    return C;
}


bool persistC(vector<mat_operator>& C) {
    // if the directory already exists, assume no need to persist
    if (!fs::create_directories(PERSIST_DIR)) {
        std::cerr << "Cannot persist operator C: directory \"" PERSIST_DIR "\" already exists" << std::endl;
        return false;
    }

    size_t i, length;
    char path[PERSIST_PATHLEN];
    FILE *f;
    
    length = basisLength();
    
    for (i = 0; i < length; ++i) {
        snprintf(path, PERSIST_PATHLEN, "%s/%zu", PERSIST_DIR, i);
        f = fopen(path, "wb");

        gsl_matrix_fwrite(f, C[i].get());
        
        fclose(f);
    }

    return true;
}


bool restoreC(vector<mat_operator>& C) {
    // if the directory doesn't exist, can't restore
    if (!fs::is_directory(PERSIST_DIR)) {
        std::cerr << "Cannot restore operator C: path \"" PERSIST_DIR "\" is not a directory" << std::endl;
        return false;
    }

    size_t i, length;
    char path[PERSIST_PATHLEN];
    FILE *f;
    gsl_matrix *Ci;
    
    length = basisLength();

    C.reserve(length);
    for (i = 0; i < length; ++i) {
        snprintf(path, PERSIST_PATHLEN, "%s/%zu", PERSIST_DIR, i);
        f = fopen(path, "rb"); 

        Ci = gsl_matrix_alloc(length, length);
        gsl_matrix_fread(f, Ci); 
        C.push_back(mat_operator(Ci, gsl_matrix_free));

        fclose(f);
    }

    return true;
}
