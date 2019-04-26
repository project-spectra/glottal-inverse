#include <cstdio>
#include "glottal.h"


double estimateOq(const std::vector<int>& gci, const std::vector<int>& goi)
{
    double mean(0.), Oq;

    for (int r = 0; r < goi.size(); ++r) {
        double t0 = gci[r+1] - gci[r];
        double to = gci[r+1] - goi[r];

        Oq = to / t0;

        //printf("Oq[%zu] = %f\n", r, Oq);

        mean += Oq;
    }

    mean /= (double) goi.size();

    return mean;
}
