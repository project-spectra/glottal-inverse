#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>

// Audio sample rate
#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 1024

// Window length for 20Hz minimum
extern const size_t WINDOW_LENGTH;

// Vector basis
#define J 7

// AM-GIF max iter count
#define MAX_ITER 1

// GNUPlot data exporting
#define GNUPLOT_FILE_IAIF_SOURCE "iaif_source.dat"
#define GNUPLOT_FILE_IAIF_SOURCE_DERIV "iaif_source_deriv.dat"
#define GNUPLOT_FILE_AMGIF_SOURCE "amgif_source.dat"
#define GNUPLOT_FILE_AMGIF_DERIV "amgif_source_deriv.dat"

#endif // CONSTANTS_H
