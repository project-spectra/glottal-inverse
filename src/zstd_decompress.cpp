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


void decompress(FILE *fileIn, FILE *fileOut)
{
    static thread_local auto strmPtr = std::shared_ptr<ZSTD_DStream>(
            ZSTD_createDStream(),
            ZSTD_freeDStream
    );
    
    ZSTD_DStream* strm(strmPtr.get());

    const size_t sizeIn = ZSTD_DStreamInSize();
    const size_t sizeOut = ZSTD_DStreamOutSize();

    std::shared_ptr<void> bufIn(new unsigned char[sizeIn]);
    std::shared_ptr<void> bufOut(new unsigned char[sizeOut]);

    const size_t initResult = ZSTD_initDStream(strm);
    zstdErrorDie(initResult, "ZSTD_initDStream");

    size_t read, toRead;
    
    toRead = initResult;

    while ((read = fread(bufIn.get(), 1, toRead, fileIn))) {
        ZSTD_inBuffer input = { bufIn.get(), read, 0 };

        while (input.pos < input.size) {
            ZSTD_outBuffer output = { bufOut.get(), sizeOut, 0 };

            toRead = ZSTD_decompressStream(strm, &output, &input);
            zstdErrorDie(toRead, "ZSTD_compressStream");
            
            fwrite(bufOut.get(), 1, output.pos, fileOut);
        }
    }

}
