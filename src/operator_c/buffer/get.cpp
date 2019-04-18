#include "operators.h"
#include "operators_buffer.h"


gsl_matrix **OperatorBuffer::get() {

    return m_mats.data();

}
