#include "gnuplot.h"


void writePlotData(gsl_function *f, size_t num, const std::string& filename) {

    // s : [0,1)
    // t : [0,winlen)
    // y = f(s)
    double s, t, y;
    size_t i;

    std::ofstream out(filename);

    for (i = 0; i < num; ++i) {
        s = (double) i / (double) num;
        t = s * WINDOW_LENGTH;
        
        y = GSL_FN_EVAL(f, s);

        out << t << " " << y << std::endl;
    }

    out.close();
}
