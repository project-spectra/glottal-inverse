#include <fstream>
#include "constants.h"
#include "gnuplot.h"


void writePlotData(gsl_vector *f, const std::string& filename) {

    // s : [0,1)
    // t : [0,winlen)
    // y = f(s)
    size_t i;

    std::ofstream out(filename);
    
    double t;

    for (i = 0; i < WINDOW_LENGTH; ++i) {
        t = i / static_cast<double>(SAMPLE_RATE);

        out << t << "," << gsl_vector_get(f, i) << std::endl;
    }

    out.close();
}
