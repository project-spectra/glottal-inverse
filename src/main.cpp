#include <iostream>
#include <cstdlib>
#include <csignal>

#include <portaudio.h>

#include "audio_in.h"
#include "iaif.h"
#include "amgif.h"
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
    window_data data;

    constexpr size_t numSamples = 2 << J;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init failure: " << Pa_GetErrorText(err) << std::endl;
        return EXIT_FAILURE;
    }

    std::atexit(terminate);

    if (!openStream(&stream, &data)) {
        std::cerr << "Exiting with error..." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << " ==== Recording ====" << std::endl;
    
    gsl_vector *me, *pe, *g;
    gsl_vector *d, *y;
    gsl_vector *source, *filter;

    me = gsl_vector_alloc(numSamples);

    std::cout << "- Computing operator L..." << std::endl;
    // generate the matrix for a low-pass filter operator
    mat_operator L(computeL());

    std::cout << "- Computing operator C..." << std::endl;
    vector<mat_operator> C(smartGetC());

    while (!stop) {
        std::cout << "- Processing one window..." << std::endl;
        // record a window
        data.frameIndex = 0;
        recordWindow(stream);

        // convert the data to doubles
        for (size_t i = 0; i < numSamples; ++i) {
            gsl_vector_set(me, i, data.recordedSamples[i]);
        }

        // normalize the data
        normalize(me);

        std::cout << "- Estimating with IAIF..." << std::endl;
        // get a first estimate with IAIF
        // undiscretize the glottal flow derivative estimate
        std::tie(pe, g) = computeIAIF(me);

        // normalize both
        normalize(pe);
        normalize(g);

        // initialize AM-GIF parameters
        double alpha, beta, tau, eps;
        alpha = 13.3354;
        beta = 8.54564;
        tau = 1.2;
        eps = 1e-5;

        std::cout << "- Estimating with AM-GIF..." << std::endl;
        // estimate with AM-GIF
        std::tie(d, y) = computeAMGIF(C, me, pe, L, alpha, beta, tau, eps);

        source = uncoords(d);
        filter = uncoords(y);

        normalize(filter);

        writePlotData(filter, GNUPLOT_FILE_SOURCE_DERIV);
        writePlotData(pe, GNUPLOT_FILE_SOURCE_FLOW);
        
        gsl_vector_free(pe);
        gsl_vector_free(g);
        gsl_vector_free(d);
        gsl_vector_free(y);
        gsl_vector_free(source);
        gsl_vector_free(filter);

        std::cout << std::endl;
    }


    std::cout << " ==== Exiting safely ====" << std::endl;

    gsl_vector_free(me);


    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
