#include "audio.h"
#include "gci_yaga_subroutines.h"

double cost_FN(const valarray& u, const int r)
{
    // 3ms of normalized amplitude segment
    static const int nrmLen(3. / 1000. * SAMPLE_RATE);
    static const int nrmOff(nrmLen / 2);

    valarray u_r(u[std::slice(r - nrmOff, nrmLen, 1)]);

    return sqrt(abs(u_r * u_r).sum());

    /*valarray w(m + p, p + 1);
    w[std::slice(0, p, 1)] = range;
    w[std::slice(m+1, p+1, 1)] = range[std::slice(p-1, p, -1)];
    
    w /= (p + 1.);

    valarray frob;
    filter_fir(w, u, frob);

    double maxNorm = frob.max();

    return 0.5 - frob[r] / maxNorm;*/
}
