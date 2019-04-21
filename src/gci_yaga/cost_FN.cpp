#include "constants.h"
#include "gci_yaga.h"

// 3ms of normalized amplitude segment
static constexpr size_t nrmLen = 3. / 1000. * SAMPLE_RATE;

// Speech segment offset
static constexpr size_t nrmOff = nrmLen / 2;

double cost_FN(const gsl_vector *u, const size_t r)
{
    valarray u_r(u->data + r - nrmOff, nrmLen);

    return sqrt(abs(u_r * u_r).sum());
}
