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

#include <string>
#include <vector>

#include <gsl/gsl_spmatrix.h>
#include <bzlib.h>

#include "constants.h"


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PERSIST_DIR "persistedOperatorC/J-" STR(J)

#define PERSIST_PATHLEN 64


using ComputeStatus = std::vector<std::pair<size_t, std::string>>;

void findComputeStatus(ComputeStatus& toLoad, ComputeStatus& toStore);

gsl_spmatrix *smartGetC(size_t mu, const std::string& path, bool load);

gsl_spmatrix *bz2readC(FILE *f);

void bz2writeC(FILE *f, gsl_spmatrix *C);


#endif // PERSIST_C_H
