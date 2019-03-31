#ifndef GNUPLOT_H
#define GNUPLOT_H


#include <fstream>
#include <gsl/gsl_vector.h>

#include "constants.h"


void writePlotData(gsl_vector *f, const std::string& filename);


#endif // GNUPLOT_H
