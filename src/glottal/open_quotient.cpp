#include <cstdio>
#include <cmath>
#include <numeric>
#include "print_iter.h"
#include "glottal.h"


double estimateOq(const std::vector<int>& gci, const std::vector<int>& goi)
{
    if (gci.empty() || goi.empty()) 
        return -1.;

    std::vector<double> Oq;


    for (int r = 0; r < gci.size() - 1; ++r) {

        // If any of the estimates used are unknown, skip.
        if (gci[r+1] < 0 || gci[r] < 0 || goi[r] < 0)
        {
            continue;
        }

        double t0 = std::abs(gci[r+1] - gci[r]);
        double to = std::abs(gci[r+1] - goi[r]);

        Oq.push_back(to / t0);
    }

    std::cout << "Oq: " << Oq;

    return std::accumulate(Oq.begin(), Oq.end(), 0.0) / (double) Oq.size();
}
