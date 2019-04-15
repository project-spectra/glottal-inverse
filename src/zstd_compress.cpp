#include <memory>
#include <zstd.h>
#include "zstd_util.h"


void compress(FILE *fileIn, FILE *fileOut, int level)
{
    static thread_local auto cctxPtr = std::shared_ptr<ZSTD_CCtx>(
            ZSTD_createCCtx(),
            ZSTD_freeCCtx
    );

    size_t sizeIn;
    std::shared_ptr<void> bufIn(readWholeFile(fileIn, &sizeIn));

    const size_t sizeOut = ZSTD_compressBound(sizeIn);
    std::shared_ptr<void> bufOut(new unsigned char[sizeOut]);

    const size_t cSize = ZSTD_compressCCtx(
            cctxPtr.get(),
            bufOut.get(), sizeOut,
            bufIn.get(), sizeIn,
            level
    );

    zstdErrorDie(cSize, "ZSTD_compressCCtx");

    fwrite(bufOut.get(), 1, cSize, fileOut);

}
