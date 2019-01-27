#include "gnuplot.h"


void writePlotData(gsl_vector *f, const std::string& filename) {

    // s : [0,1)
    // t : [0,winlen)
    // y = f(s)
    size_t i;

    std::ofstream out(filename);

    for (i = 0; i < f->size; ++i) {
        out << (i * SAMPLE_PER) << ","
            << gsl_vector_get(f, i) << std::endl;
    }

    out.close();
}
