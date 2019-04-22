#include "filter.h"

void filter_fir(const valarray& b, const valarray& x, valarray& res) {
    size_t m, n;
    double data;

    res.resize(x.size());

    for (m = 0; m < x.size(); ++m) {
        data = 0.0;
        
        for (n = 0; n < b.size() && n <= m; ++n) {
            data += b[n] * x[m - n];
        }

        res[m] = data;
    }
}

void filter_iir(const valarray& b, const valarray& a, const valarray& x, valarray& res) {
    size_t m, n;
    double data;
    
    res.resize(x.size());

    for (m = 0; m < x.size(); ++m) {
        data = 0.0;
        
        for (n = 0; n < b.size() && n <= m; ++n) {
            data += b[n] * x[m - n];
        }
        
        for (n = 1; n < a.size() && n <= m; ++n) {
            data -= a[n] * res[m - n];
        }

        res[m] = data;
    }
}


