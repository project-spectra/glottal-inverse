#include "lpc.h"


valarray lpcar2rf(const valarray& ar) {
    int p = ar.size();

    valarray rf;

    if (p == 1) {
        rf = { 1 };
    }
    else {
        rf = ar;
        
        if (rf[0] != 1.) {
            rf /= rf[0];
        }

        for (int j = p - 2; j >= 1; --j) {
            double k = rf[j + 1];
            double d = 1. / (1. - k * k);

            rf[std::slice(1, j - 1, 1)] = (valarray(rf[std::slice(1, j - 1, 1)])
                                            - k * valarray(rf[std::slice(j - 1, j - 1, -1)])) * d;
        }
            
    }

	return rf;
}
