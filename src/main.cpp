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
        sizeof(window_data) + basis_length() * sizeof(sample)
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
    gsl_matrix *L(computeL());

    std::cout << "- Computing operator C..." << std::endl;
    vector<mat_operator> C(computeC());

    while (!stop) {
        std::cout << "- Processing one window..." << std::endl;

        // record a window
        data->frameIndex = 0;
        recordWindow(stream);

        // convert the data to doubles
        for (size_t i = 0; i < WINDOW_LENGTH; ++i) {
            gsl_vector_set(me, i, data->recordedSamples[i]);
        }
        normalize(me);

        //std::cout << "- Estimating with IAIF..." << std::endl;

        // get a first estimate with IAIF
        std::tie(dg, g) = computeIAIF(me);
        normalize(g);

        // initialize AM-GIF parameters
        double alpha, beta, tau, eps;
        alpha = 5;
        beta = 3;
        tau = 1.2;
        eps = 1e-3;

        //std::cout << "- Estimating with AM-GIF..." << std::endl;

        // estimate with AM-GIF
        std::tie(source, filter) = computeAMGIF(C, me, g, L, alpha, beta, tau, eps);
        normalize(source);
        normalize(filter);

        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(source, GNUPLOT_FILE_AMGIF_SOURCE);

        gsl_vector_free(dg);
        gsl_vector_free(g);
        gsl_vector_free(source);
        gsl_vector_free(filter);
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    gsl_vector_free(me);
    gsl_matrix_free(L);
    free(data);

    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}

#endif // PRECOMP
