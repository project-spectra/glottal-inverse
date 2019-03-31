#ifndef PRECOMP

#include <iostream>
#include <cstdlib>
#include <csignal>

#include <portaudio.h>

#include "audio_in.h"
#include "iaif.h"
#include "amgif.h"
#include "linalg.h"
#include "persist_c.h"
#include "gnuplot.h"


volatile sig_atomic_t stop;

void terminate();
void inthand(int signum) {
    (void) signum;
    stop = 1;
}


int main() {
    PaError err;
    PaStream *stream;
    window_data *data;
  
    data = static_cast<window_data *>(malloc(
        sizeof(window_data) + WINDOW_LENGTH * sizeof(sample)
    ));

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failure: " << Pa_GetErrorText(err) << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(terminate);

    if (!openStream(&stream, data)) {
        std::cerr << "Exiting with error..." << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << " ==== Recording ====" << std::endl;
   
    gsl_vector *me, *dg, *g;
    gsl_vector *source, *filter;

    me = gsl_vector_alloc(WINDOW_LENGTH);

    std::cout << "- Computing operator L..." << std::endl;
    // generate the matrix for a low-pass filter operator
    mat_operator L(computeL());

    std::cout << "- Computing operator C..." << std::endl;
    vector<mat_operator> C(smartGetC());

    while (!stop) {
        std::cout << "- Processing one window..." << std::endl;

        // record a window
        data->frameIndex = 0;
        recordWindow(stream);

        // convert the data to doubles
        for (size_t i = 0; i < WINDOW_LENGTH; ++i) {
            gsl_vector_set(me, i, data->recordedSamples[i]);
        }

        // normalize the data
        double max = gsl_vector_get(me, gsl_blas_idamax(me));
        gsl_vector_scale(me, 1. / abs(max));

        std::cout << "- Estimating with IAIF..." << std::endl;

        // get a first estimate with IAIF
        std::tie(dg, g) = computeIAIF(me);
        
        // initialize AM-GIF parameters
        double alpha, beta, tau, eps;
        alpha = 1.3354;
        beta = 9.54564;
        tau = 1.2;
        eps = 1e-10;

        std::cout << "- Estimating with AM-GIF..." << std::endl;

        // estimate with AM-GIF
        std::tie(source, filter) = computeAMGIF(C, me, g, L, alpha, beta, tau, eps);

        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(source, GNUPLOT_FILE_AMGIF_SOURCE);

        gsl_vector_free(dg);
        gsl_vector_free(g);
        gsl_vector_free(source);
        gsl_vector_free(filter);

        break;
        std::cout << std::endl;
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    gsl_vector_free(me);
    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}

#endif // PRECOMP
