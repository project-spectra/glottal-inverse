#include "gci_sedreams.h"


void findZeroCrossings(const valarray& signal, std::vector<int>& pos, std::vector<int>& neg)
{

    double cur, next;

    next = signal[0];

    for (int t = 0; t < signal.size() - 1; ++t) {
        cur = next;
        next = signal[t + 1];
        
        // sign change or null
        if (cur * next <= 0) {
            // positive going or null
            if (next >= 0) {
                pos.push_back(t);
            } else {
                neg.push_back(t);
            }
        }

    }

}
