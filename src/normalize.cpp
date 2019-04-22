#include <numeric>
#include "normalize.h"

void normalize(valarray& f) {

    double maxAmp = abs(f).max();

    if (std::isfinite(maxAmp) && maxAmp > 1e-10) {
        f /= maxAmp;
    }

}

