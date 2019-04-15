#include <memory>
#include <zstd.h>
#include "zstd_util.h"


void decompress(FILE *fileIn, FILE *fileOut)
{
    static thread_local auto dctxPtr = std::shared_ptr<ZSTD_DCtx>(
            ZSTD_createDCtx(),
            ZSTD_freeDCtx
    );

    size_t sizeIn;
    std::shared_ptr<void> bufIn(readWholeFile(fileIn, &sizeIn));

    const size_t sizeOut = ZSTD_getFrameContentSize(bufIn.get(), sizeIn);
    std::shared_ptr<void> bufOut(new unsigned char[sizeOut]);

    const size_t dSize = ZSTD_decompressDCtx(
            dctxPtr.get(),
            bufOut.get(), sizeOut,
            bufIn.get(), sizeIn
    );

    fwrite(bufOut.get(), 1, dSize, fileOut);

}

