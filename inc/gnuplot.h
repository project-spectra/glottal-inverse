#ifndef GNUPLOT_H
#define GNUPLOT_H


#include <string>
#include <gsl/gsl_vector.h>


void writePlotData(gsl_vector *f, const std::string& filename);


#endif // GNUPLOT_H
