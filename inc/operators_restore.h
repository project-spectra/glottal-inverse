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

#include <zstd.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

constexpr auto PERSIST_DIR = "persistedOperatorC/J-" STR(J);

#undef STR_HELPER
#undef STR

void compress(FILE *in, FILE *out, int level);
void decompress(FILE *in, FILE *out);


#endif // OPERATORS_RESTORE_H
