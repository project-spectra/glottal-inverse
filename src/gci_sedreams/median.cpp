#include <algorithm>
#include "gci_sedreams.h"


double median(std::vector<double>& x) {
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
