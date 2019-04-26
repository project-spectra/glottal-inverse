#ifndef GNUPLOT_H
#define GNUPLOT_H


#include <string>
#include <valarray>

using valarray = std::valarray<double>;


void writePlotData(const valarray& f, const std::string& filename);


#define GNUPLOT_FILE_SPEECH "speech.dat"
#define GNUPLOT_FILE_SOURCE "glottal_source.dat"
#define GNUPLOT_FILE_SOURCE_DERIV "glottal_source_deriv.dat"


#endif // GNUPLOT_H
