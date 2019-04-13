#ifndef OPERATORS_RESTORE_H
#define OPERATORS_RESTORE_H


#include <iostream>

#if __cplusplus < 201703L
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#include <zlib.h>
#include "zpipe.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

constexpr auto PERSIST_DIR = "persistedOperatorC/J-" STR(J);

#undef STR_HELPER
#undef STR


#endif // OPERATORS_RESTORE_H
