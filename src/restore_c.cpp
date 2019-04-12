#include "restore_c.h"
#include "amgif.h"

#include <sstream>

void findComputeStatus(ComputeStatus& toLoad, ComputeStatus& toStore) {
    bool dirExists = fs::is_directory(PERSIST_DIR);

    std::ostringstream out;
    out << PERSIST_DIR << '/';
    
    auto sep = out.tellp();

    for (size_t mu = 0; mu < length; ++mu) {
        out.seekp(sep) << "C_" << mu << ".gz" << std::flush;
        
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
    gsl_spmatrix *C;

    fs::create_directories(PERSIST_DIR);

    // remove the .gz
    const std::string pathCmp(path, 0, path.length() - 3);

    if (load) {
        // if the directory already exists, simply restore.
        #pragma omp critical
        C = cpReadMat(path.c_str(), pathCmp.c_str());
    } else {
        // else, compute and then persist.

        C = gsl_spmatrix_alloc(length, length);

        computeSingleC(C, mu);
        
        auto Cc = gsl_spmatrix_ccs(C);
        gsl_spmatrix_free(C);
        C = Cc;
        
        #pragma omp critical
        cpWriteMat(path.c_str(), pathCmp.c_str(), C);
    }

    return C;
}

