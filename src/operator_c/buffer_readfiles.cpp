#include "operators.h"
#include "operators_restore.h"
#include "operators_buffer.h"
#include "zstd_util.h"


void OperatorBuffer::readCompressedFiles()
{

    size_t mu;

    m_data.resize(basisLength);
    
    for (mu = 0; mu < basisLength; ++mu) {
        std::string filename = std::string(PERSIST_DIR)
                            + "/C_" + std::to_string(mu) + ".zstr";

        FILE *file = fopen(filename.c_str(), "rb");
        
        size_t totalSize;
        void *buffer = readWholeFile(file, &totalSize);
        
        fclose(file);

        // store in vector
        m_data[mu] = std::make_pair(totalSize, buffer);
    }

}
