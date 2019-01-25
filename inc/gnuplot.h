#ifndef GNUPLOT_H
#define GNUPLOT_H


#include <fstream>
#include <gsl/gsl_math.h>

#include "constants.h"


void writePlotData(gsl_function *f, size_t num, const std::string& filename);


#endif // GNUPLOT_H
