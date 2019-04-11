#include "restore_c.h"
#include "amgif.h"

#include <sstream>

void findComputeStatus(ComputeStatus& toLoad, ComputeStatus& toStore) {
    bool dirExists = fs::is_directory(PERSIST_DIR);

    std::ostringstream out;
    out << PERSIST_DIR << '/';
    
    auto sep = out.tellp();

    for (size_t mu = 0; mu < length; ++mu) {
        out.seekp(sep) << "C_" << mu << ".z" << std::ends;
        
        const auto path = out.str();
        
        auto val = std::make_pair(mu, path);

        if (dirExists && fs::exists(path) && fs::is_regular_file(path)) {
            toLoad.push_back(val);
        } else {
            toStore.push_back(val);
        }
    }
}

gsl_spmatrix *smartGetC(size_t mu, const std::string& path, bool load) {
    FILE *f;
    gsl_spmatrix *C;

    if (load) {
        // if the directory already exists, simply restore.
        f = fopen(path.c_str(), "rb");

        C = cpReadMat(f);

        fclose(f);
    } else {
        // else, compute and then persist.
        fs::create_directories(PERSIST_DIR);

        C = gsl_spmatrix_alloc(length, length);

        computeSingleC(C, mu);

        f = fopen(path.c_str(), "wb");

        auto Cp = cpWriteMat(f, C);
        gsl_spmatrix_free(C);
        C = Cp;

        fclose(f);
    }
    
    return C;
}

