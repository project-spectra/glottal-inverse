#ifndef CONSTANTS_H
#define CONSTANTS_H


// Audio sample rate
#define SAMPLE_RATE 48000
#define NUM_CHANNELS 1
#define FRAMES_PER_BUFFER 1024
#define WINDOW_LENGTH .04

// Vector basis
#define J 6

// AM-GIF max iter count
#define MAX_ITER 1

// Integration workspace size.
#define INTEGRATE_EPSABS 0
#define INTEGRATE_EPSREL 1e-7
#define INTEGRATE_WORK 14

// Monte-Carlo integration calls.
#define INTEGRATE_NCALLS 2000

// GNUPlot data exporting
#define GNUPLOT_FILE_SOURCE_FLOW "est_source_flow.dat"
#define GNUPLOT_FILE_SOURCE_DERIV "est_source_deriv.dat"
#define GNUPLOT_NUM (SAMPLE_RATE * WINDOW_LENGTH)

#endif // CONSTANTS_H
