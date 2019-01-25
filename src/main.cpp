#include <iostream>
#include <cstdlib>
#include <csignal>

#include <portaudio.h>

#include "audio_in.h"
#include "iaif.h"
#include "amgif.h"
#include "persist_c.h"
#include "interp_sample.h"


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

    constexpr size_t numSamples = WINDOW_LENGTH * SAMPLE_RATE * NUM_CHANNELS;

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
    
    gsl_vector *input;
    gsl_function me, pe;

    input = gsl_vector_alloc(numSamples);
    me.function = pe.function = interp_sample_eval;

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
            gsl_vector_set(input, i, data.recordedSamples[i]);
        }

        std::cout << "- Estimating with IAIF..." << std::endl;
        // get a first estimate with IAIF
        // undiscretize the glottal flow derivative estimate
        gsl_vector *pe_discr = computeIAIF(input, numSamples);
        void *pe_interp = interp_sample(pe_discr, numSamples, true);
        pe.params = pe_interp;

        // undiscretize the input samples for AM-GIF
        void *me_interp = interp_sample(input, numSamples, false);
        me.params = me_interp;

        // initialize AM-GIF parameters
        double alpha, beta, tau, eps;
        alpha = 2.3354;
        beta = 0.54564;
        tau = 0.8;
        eps = .01;

        std::cout << "- Estimating with AM-GIF..." << std::endl;
        // estimate with AM-GIF
        gsl_vector *x, *y;
        std::tie(x, y) = computeAMGIF(C, &me, &pe, L, alpha, beta, tau, eps);

        free(pe_interp);
        free(me_interp);

        std::cout << std::endl;
    }


    std::cout << " ==== Exiting safely ====" << std::endl;

    gsl_vector_free(input);


    return EXIT_SUCCESS;
}

void terminate() {
    Pa_Terminate();
}
