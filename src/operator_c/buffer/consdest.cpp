#include "operators.h"
#include "operators_buffer.h"


OperatorBuffer::OperatorBuffer()
{ 
    // read into m_data
    readCompressedFiles();

    decompressFiles();
}

OperatorBuffer::~OperatorBuffer()
{
    for (auto& entry : m_data) {
        free(entry.second);
    }

    for (auto& mat : m_mats) {
        gsl_matrix_free(mat);
    }
}


