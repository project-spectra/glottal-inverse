#ifndef GNUPLOT_H
#define GNUPLOT_H


#include <string>
#include <valarray>

using valarray = std::valarray<double>;


void writePlotData(const valarray& f, const std::string& filename);


#endif // GNUPLOT_H
