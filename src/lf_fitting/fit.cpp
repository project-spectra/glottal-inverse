#include <set>
#include "audio.h"
#include "lf_fitting.h"


void lf_fit(const valarray& gfd, const std::vector<int>& GCIs, double T0, std::vector<lf_params>& out)
{
    // Fundamental period in samples
    const int N = std::round(T0 * SAMPLE_RATE);

    // Filter the GCIs so there's enough room for estimation.
    for (int te : GCIs) {
        if (te - N >= 0 && te + N < gfd.size()) {
            // LF-fit the current GCI
            lf_params lf;

            if (lf_fit_period(gfd, te, T0, lf)) {
                out.push_back(lf);
            }
        }
    }
}
