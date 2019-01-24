#ifndef CONSTANTS_H
#define CONSTANTS_H


// Audio sample rate
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define FRAMES_PER_BUFFER 1024
#define WINDOW_LENGTH .2

// Vector basis
#define J 4
#define TAU 1.2

// Integration workspace size.
#define INTEGRATE_EPSABS 0
#define INTEGRATE_EPSREL 1e-7
#define INTEGRATE_WORK 10

// Monte-Carlo integration calls.
#define INTEGRATE_NCALLS 10000

#endif // CONSTANTS_H
