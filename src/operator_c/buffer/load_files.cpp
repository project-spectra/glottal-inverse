#include "operators.h"
#include "operators_restore.h"
#include "operators_buffer.h"
#include "zstd_util.h"


void OperatorBuffer::readCompressedFiles()
{

    size_t mu;

    m_data.resize(basisLength);

    size_t tot(0);

    for (mu = 0; mu < basisLength; ++mu) {
        std::string filename = std::string(PERSIST_DIR)
                            + "/C_" + std::to_string(mu) + ".zstd";

        FILE *file = fopen(filename.c_str(), "rb");
        
        size_t totalSize;
        void *buffer = readWholeFile(file, &totalSize);

        tot += totalSize;

        fclose(file);

        // store in vector
        m_data[mu] = std::make_pair(totalSize, buffer);
    }
 
    std::cout << "totalUsage from files = " << tot / 1024. << "KiB" << std::endl;

}
