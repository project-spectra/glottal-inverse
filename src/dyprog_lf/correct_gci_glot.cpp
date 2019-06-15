#include "audio.h"
#include "dyprog_lf.h"

void correct_GCI_glot(const valarray& dg, std::vector<int>& GCIs) {

    static constexpr double searchRate = 0.2;
    static constexpr double maxPulseLen = 0.03;

    std::vector<int> correctedGCIs;

    if (GCIs.size() > 1) {
        for (int n = 0; n < GCIs.size(); ++n) {
            double pulseLen;

            if (n == 0) {
                pulseLen = GCIs[n + 1] - GCIs[n];
            } else {
                pulseLen = GCIs[n] - GCIs[n - 1];
            }

            if (pulseLen < maxPulseLen * SAMPLE_RATE) {
                int start, finish;

                start = std::max<int>(GCIs[n] - round(pulseLen * searchRate), 0);
                finish = std::min<int>(GCIs[n] + round(pulseLen * searchRate), dg.size() - 1);

                double min = HUGE_VAL;
                int idx = -1;

                for (int k = start; k <= finish; ++k) {
                    if (dg[k] < min) {
                        min = dg[k];
                        idx = k;
                    }
                }

                if (idx >= 0) {
                    correctedGCIs.push_back(idx);     
                }
            }
            else {
                correctedGCIs.push_back(n);
            }
        }
    }

    GCIs = std::move(correctedGCIs);
}
