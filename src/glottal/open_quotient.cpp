#include <cstdio>
#include "glottal.h"


double estimateOq(const std::vector<int>& gci, const std::vector<int>& goi)
{
    if (gci.empty() || goi.empty()) 
        return -1.;

    double mean(0.), Oq;

    int r;

    // must have GCI > GCI
    r = (gci[0] > goi[0] ? 1 : 0);

    for (; r < goi.size() && r + 1 < gci.size(); ++r) {
        double t0 = gci[r+1] - gci[r];
        double to = gci[r] - goi[r];

        Oq = to / t0;

        printf("Oq[%d] = %f\n", r, Oq);

        mean += Oq;
    }

    mean /= (double) goi.size();

    return mean;
}
