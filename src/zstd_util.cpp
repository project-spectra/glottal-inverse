#include "zstd_util.h"


unsigned char *readWholeFile(FILE *file, size_t *totalSize)
{
    // get total size
    fseek(file, 0, SEEK_END);
    *totalSize = ftell(file);
    rewind(file);

    // create buffer
    unsigned char *buffer = new unsigned char[*totalSize];

    // read whole file
    fread(buffer, 1, *totalSize, file);

    return buffer;
}

