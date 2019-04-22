#include "constants.h"
#include "gci_yaga_subroutines.h"

// 3ms of normalized amplitude segment
static constexpr size_t nrmLen = 3. / 1000. * SAMPLE_RATE;

// Speech segment offset
static constexpr size_t nrmOff = nrmLen / 2;

double cost_FN(const valarray& u, const size_t r)
{
    valarray u_r(u[std::slice(r - nrmOff, nrmLen, 1)]);

    return sqrt(abs(u_r * u_r).sum());
}
