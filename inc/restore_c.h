#ifndef PERSIST_C_H
#define PERSIST_C_H


#include <iostream>

#if __cplusplus < 201703L
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <gsl/gsl_spmatrix.h>
#include <zlib.h>

#include "constants.h"
#include "zpipe.h"


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PERSIST_DIR "persistedOperatorC/J-" STR(J)

#define PERSIST_PATHLEN 64

using std::make_unique;

using ComputeStatus = std::vector<std::pair<size_t, std::string>>;

void findComputeStatus(ComputeStatus& toLoad, ComputeStatus& toStore);

gsl_spmatrix *smartGetC(size_t mu, const std::string& path, bool load);

gsl_spmatrix *cpReadMat(const char *pathRaw, const char *pathCompressed);

void cpWriteMat(const char *pathRaw, const char *pathCompressed, gsl_spmatrix *mat);


#endif // PERSIST_C_H
