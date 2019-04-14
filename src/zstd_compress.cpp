#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <zstd.h>


static void zstdErrorDie(size_t result, const char *func) {
    if (ZSTD_isError(result)) {
        std::cerr << func << "() error: " << ZSTD_getErrorName(result) << std::endl;
        exit(EXIT_FAILURE);
    }
}


void compress(FILE *fileIn, FILE *fileOut, int level)
{
    static thread_local auto strmPtr = std::shared_ptr<ZSTD_CStream>(
            ZSTD_createCStream(),
            ZSTD_freeCStream
    );
    
    ZSTD_CStream* strm(strmPtr.get());

    const size_t sizeIn = ZSTD_CStreamInSize();
    const size_t sizeOut = ZSTD_CStreamOutSize();

    std::shared_ptr<void> bufIn(new unsigned char[sizeIn]);
    std::shared_ptr<void> bufOut(new unsigned char[sizeOut]);

    const size_t initResult = ZSTD_initCStream(strm, level);
    zstdErrorDie(initResult, "ZSTD_initCStream");

    size_t read, toRead;
    
    toRead = sizeIn;

    while ((read = fread(bufIn.get(), 1, toRead, fileIn))) {
        ZSTD_inBuffer input = { bufIn.get(), read, 0 };

        while (input.pos < input.size) {
            ZSTD_outBuffer output = { bufOut.get(), sizeOut, 0 };

            toRead = ZSTD_compressStream(strm, &output, &input);
            zstdErrorDie(toRead, "ZSTD_compressStream");

            if (toRead > sizeIn)
                toRead = sizeIn;

            fwrite(bufOut.get(), 1, output.pos, fileOut);
        }
    }

    ZSTD_outBuffer output = { bufOut.get(), sizeOut, 0 };
    
    const size_t remainingToFlush = ZSTD_endStream(strm, &output);
    if (remainingToFlush > 0) {
        std::cerr << "Not fully flushed" << std::endl;
        exit(EXIT_FAILURE);
    }

    fwrite(bufOut.get(), 1, output.pos, fileOut);

}
