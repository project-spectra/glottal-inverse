#ifndef ZSTD_UTIL_H
#define ZSTD_UTIL_H


#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <zstd.h>


unsigned char *readWholeFile(FILE *file, size_t *totalSize);


inline void zstdErrorDie(size_t result, const char *func) {
    if (ZSTD_isError(result)) {
        fprintf(stderr, "%s() error: %s\n", func, ZSTD_getErrorName(result));
        exit(EXIT_FAILURE);
    }
}


#endif // ZSTD_UTIL_H
