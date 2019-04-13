#ifndef PRECOMP

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>

#include <portaudio.h>

#include "audio_in.h"
#include "iaif.h"
#include "amgif.h"
#include "linalg.h"
#include "operators.h"
#include "gnuplot.h"


volatile std::atomic<bool> stop;

void terminate();
void inthand(int signum) {
    (void) signum;
    stop = true;
}


int main() {
    
    computeC();
    return 1;
/*
    PaError err;
    PaStream *stream;
    window_data *data;
  
    data = static_cast<window_data *>(malloc(
        sizeof(window_data) + length * sizeof(sample)
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
   
    gsl_vector_view md_view;

    gsl_vector *md, *dg, *g;
    gsl_vector *m, *f, *p;

    md_view = gsl_vector_view_array(data->recordedSamples, WINDOW_LENGTH);
    md = &md_view.vector;

    std::cout << "- Computing operator L..." << std::endl;
    // generate the matrix for a low-pass filter operator
    mat_operator L(computeL());

    std::cout << "- Computing operator C..." << std::endl;
    vector<mat_operator> C(computeC());

    while (!stop) {
        std::cout << "- Processing one window..." << std::endl;

        // record a window
        data->frameIndex = 0;
        recordWindow(stream);
        normalize(md);

        std::cout << "- Estimating with IAIF..." << std::endl;

        // get a first estimate with IAIF
        std::tie(dg, g) = computeIAIF(md);
        normalize(dg);
        normalize(g);

        // initialize AM-GIF parameters
        double alpha, beta, tau, eps;
        alpha = 5;
        beta = 3;
        tau = 1.2;
        eps = 1e-4;

        std::cout << "- Estimating with AM-GIF..." << std::endl;

        // estimate with AM-GIF
        //  m: signal function
        //  f: input function
        //  p: charac function
        std::tie(m, f, p) = computeAMGIF(C, md, dg, L, alpha, beta, tau, eps);
        normalize(m);
        normalize(f);
        normalize(p);

        writePlotData(g, GNUPLOT_FILE_IAIF_SOURCE);
        writePlotData(f, GNUPLOT_FILE_AMGIF_SOURCE);

        gsl_vector_free(dg);
        gsl_vector_free(g);
        gsl_vector_free(m);
        gsl_vector_free(f);
        gsl_vector_free(p);
    }

    std::cout << " ==== Exiting safely ====" << std::endl;

    free(data);

    return EXIT_SUCCESS;*/
}

void terminate() {
    Pa_Terminate();
}

#endif // PRECOMP
