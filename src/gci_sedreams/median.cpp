#include <algorithm>
#include "gci_sedreams.h"


int median(std::vector<int>& x) {
    const size_t len(x.size());

    if (len == 0) {
        return 0.;
    }

    std::sort(x.begin(), x.end());

    if (len % 2 == 1) {
        return x[len / 2];
    } else {
        return .5 * (x[len / 2 - 1] + x[len / 2]);
    }
}
