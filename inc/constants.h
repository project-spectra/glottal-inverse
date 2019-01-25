#ifndef CONSTANTS_H
#define CONSTANTS_H


// Audio sample rate
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define FRAMES_PER_BUFFER 1024
#define WINDOW_LENGTH .06

// Vector basis
#define J 5

// AM-GIF max iter count
#define MAX_ITER 10

// Integration workspace size.
#define INTEGRATE_EPSABS 0
#define INTEGRATE_EPSREL 1e-6
#define INTEGRATE_WORK 10

// Monte-Carlo integration calls.
#define INTEGRATE_NCALLS 5000

// GNUPlot data exporting
#define GNUPLOT_FILE_SOURCE "est_source.dat"
#define GNUPLOT_FILE_FILTER "est_filter.dat"
#define GNUPLOT_NUM (SAMPLE_RATE * WINDOW_LENGTH)

#endif // CONSTANTS_H
