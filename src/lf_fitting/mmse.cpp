#include "lf_fitting.h"


double lf_mmse(const valarray& r0, const valarray& r) {

    double sum(0.0), y;

    for (int i = 0; i < r0.size(); ++i) {
        y = r0[i] - r[i];
        sum += y * y;
    }

    return sum;

}
