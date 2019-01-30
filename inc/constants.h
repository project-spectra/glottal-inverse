#ifndef CONSTANTS_H
#define CONSTANTS_H


// Audio sample rate
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1024

// Vector basis
#define J 10

// AM-GIF max iter count
#define MAX_ITER 3

// Integration workspace size.
#define INTEGRATE_EPSABS 0
#define INTEGRATE_EPSREL 1e-6
#define INTEGRATE_WORK 10

// Monte-Carlo integration calls.
#define INTEGRATE_NCALLS 5000

// GNUPlot data exporting
#define GNUPLOT_FILE_SOURCE_DERIV "est_source_deriv.dat"
#define GNUPLOT_FILE_SOURCE_FLOW "est_source_flow.dat"
#define GNUPLOT_FILE_FILTER "est_filter.dat"

#endif // CONSTANTS_H
