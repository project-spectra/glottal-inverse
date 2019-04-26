#include "audio.h"
#include "gci_yaga_subroutines.h"

double cost_FN(const valarray& u, const size_t r)
{
    // 3ms of normalized amplitude segment
    static const int nrmLen(3. / 1000. * SAMPLE_RATE);
    static const int nrmOff(nrmLen / 2);


    valarray u_r(u[std::slice(r - nrmOff, nrmLen, 1)]);

    return sqrt(abs(u_r * u_r).sum());
}
